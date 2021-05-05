#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <thread>

using namespace std;

#define BUF 256
#define PORT 6542

class Client {
private:
    int port;
    char* ip;
	int cSocket;
	char buffer[BUF];
    char rBuffer[BUF];
	struct sockaddr_in address;
	int size;
    bool active;

public:
	Client() {
        Client::port = PORT;
	}

	Client(char* ip, char* port) {
        Client::ip = ip;
        Client::port = atoi(port);
        active = true;
	}

    /*
    * Connect to the Server
    */
	int connectToServer() {
        printf("IP: %s, PORT: %d", ip, port);

        if ((cSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
            perror("Socket error");
            return 1;
        }

        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        inet_aton(ip, &address.sin_addr);

        if (connect(cSocket, (struct sockaddr*)&address, sizeof(address)) == 0) {
            printf("Connection with server (%s) established\n", inet_ntoa(address.sin_addr));
        } else {
            perror("Connect error - no server available");
            return 1;
        }

        printf("Enter Command: \n");

        thread t = thread(&Client::recvMsg, this); //Start thread for reading messages from Server
        sendMsg();

        t.join();

        close(cSocket);
        return 0;
	}

    /*
    * Sends a inputed line to the Server
    * 
    * returns 0 on Succes
    */
    int sendMsg() {
        while (active) {
            fgets(buffer, BUF, stdin);

            for (int i = 0; i < sizeof(buffer); ++i)
                if (buffer[i] == ' ')
                    buffer[i] = '^';

            if (active)
                if (buffer[0] == ':' && buffer[1] == ':')
                    sendFile(buffer);

                send(cSocket, buffer, strlen(buffer), 0);

        }

        return 0;
    }

    /*
    * Recives and prints messages from Server
    * 
    * returns 0 on Success
    */
    int recvMsg() {
        do {
            size = recv(cSocket, rBuffer, BUF - 1, 0);

            if (size > 0) {
                rBuffer[size] = '\0';
                printf("\n%s\n", rBuffer);

                if (strncmp(rBuffer, "Quit", 4) == 0)
                    break;

                printf("\nEnter Command: \n");
            }

        } while (strncmp(rBuffer, "Quit", 4) != 0);

        active = false;

        return 0;
    }

    void sendFile(char *buffer) {
        memmove(buffer, buffer + 2, strlen(buffer));
        char fdata[1024], fname[32];

        FILE *fptr = fopen("/home/abru/mail/tfile", "r");

        sprintf(fname, "::%s", buffer);
        printf("%s\n", buffer);

        if (fptr != 0) {
            send(cSocket, fname, sizeof(fname), 0);

            while (fgets(fdata, 256, fptr) != NULL) {
                    if (send(cSocket, fdata, sizeof(fdata), 0) == -1) {
                        perror("[-]Error in sending file.");
                        exit(1);
                    }
            }

            send(cSocket, ";;", sizeof(";;"), 0);

            fclose(fptr);
        } else {
            perror("[-]Error in reading file.");
        }    }
};