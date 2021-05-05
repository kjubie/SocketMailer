#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <thread>

#include "ClientHandler.cpp"

#define BUF 256
#define PORT 6542

using namespace std;

class Server {
private:
    mutex mtx;

	int port;
    int create_socket, new_socket = 1;

    vector<thread> clients;
    vector<ClientHandler*> chs;

	socklen_t addrlen;
	struct sockaddr_in address, cliaddress;
    int closed = 0;

public:
	Server() {
        Server::port = PORT;
	}

	Server(char* port) {
        Server::port = atoi(port);
	}

    ~Server(){
        for (int i = 0; i < clients.size(); i++)
            clients[i].join();

        chs.clear();
        clients.clear();
    }

    /*
    * Creating the Socket
    */
    int createSocket() {
        create_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(create_socket, (struct sockaddr*)&address, sizeof(address)) != 0) {
            perror("bind error");
            return 1;
        }
        listen(create_socket, 5);

        addrlen = sizeof(struct sockaddr_in);

        do {
            printf("Waiting for connections on %d...\n", port);

            clients.push_back(thread(&Server::createCH, this, accept(create_socket, (struct sockaddr*)&cliaddress, &addrlen)));
            printf("Client connected from %s:%d...\n", inet_ntoa(cliaddress.sin_addr), ntohs(cliaddress.sin_port));

        } while (1);

        close(create_socket);
        return 0;
    }

    void createCH(int socket) {
        ClientHandler* ch = new ClientHandler(socket);
        chs.push_back(ch);
        ch->recvMsg();
        ch->sendMsg("Quit");
        delete(ch);
    }
};