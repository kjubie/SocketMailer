#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "FileHandler.cpp"

#define BUF 256

using namespace std;

class ClientHandler {
private:
    char user[64];
	int socket;
    FileHandler* fh;
    bool active;

    char buffer[BUF];
    char sBuffer[BUF];
    char msg[64][1024];
    size_t size;

public:
	ClientHandler(int socket) {
        ClientHandler::socket = socket;
        fh = new FileHandler();

        active = true;

        memset(buffer, 0, BUF);
        memset(sBuffer, 0, BUF);
	}

    /*
    * Send message to Client
    *
    * msg -> Message to send
    */
    int sendMsg(char* msg) {
        send(socket, msg, strlen(msg), 0);

        return 0;
    }

    /*
    * Recieve message from Client
    */
    int recvMsg() {
        int i = 0;

        while (active) {
            size = recv(socket, buffer, BUF - 1, 0);

            printf("%s\n", buffer);

            if (size > 0) {
                buffer[size] = '\0';
                if (getMsg(i) == 0) { //Test if Command is complete
                    i = -1;
                    doCommand();
                }
            } else if (size == 0) {
                printf("Client closed remote socket\n");
                break;
            } else {
                perror("recv error");
                break;
            }

            ++i;
        }

        return 0;
    }

    /*
    * Saves line from buffer and tests if message ended with "."
    *
    * Params:
    *   i -> Position counter for msg array
    *
    * Returns:
    *   0 -> When Client send \n.\n
    *   1 -> When Client didnt send \n.\n
    */
    int getMsg(int i) {
        formatBuffer();

        if (strncmp(buffer, "::", 2) == 0)
            recvFile();

        strcpy(msg[i], buffer); //Copy buffer to msg[i]

        if (strcmp(buffer, ".") == 10) {
            return 0;
        }

        memset(buffer, 0, BUF);
        return 1;
    }

    int recvFile() {
        char *filepath, *filename;

        strcpy(filename, buffer);

        sprintf(filepath, "/home/abru/mail/attFiles/%s", filename);

        printf("Filepath: %s\n", filepath);

        FILE *fptr = fopen(filepath, "w");

        if (fptr == 0)
            return -1;
        else
            while (1) {
                printf("recvFile TEST\n");
                size = recv(socket, buffer, BUF, 0);

                if (strncmp(buffer, ";;", 2)) {
                    fclose(fptr);
                    break;
                    return 0;
                }

                fputs(buffer, fptr);

                bzero(buffer, BUF);
            }
    }

    /*
    * Identifies and executes the command send by the Client. Response with a message depending on the outcome of the command.
    */
    int doCommand() {
        formatMsg();

        if (strncmp(msg[0], "send", 4) == 0) {
            switch (write()) {
            case 0:
                sendMsg("Message sent!");
                break;

            case -1:
                sendMsg("Could not send message You must provide a user!");
                break;

            case -2:
                sendMsg("Could not send message: You must provide a header!");
                break;

            case -3:
                sendMsg("Could not send message: You must provide a text!");
                break;

            case -4:
                sendMsg("Could not send message: Username can only be 8 characters long!");
                break;

            default:
                sendMsg("Could not send message: Unidentified Error");
                break;
            }
        } else if (strncmp(msg[0], "list", 4) == 0) {
            char files[512];
            switch (listF(files)) {
            case 0:
                sendMsg(files);
                break;

            case -1:
                sendMsg("Could not list files: Invalid user!");
                break;

            case -2:
                sendMsg("Could not list files: User does not exist!");
                break;

            case -5:
                sendMsg("Could not list files: User has no Messages!");
                break;

            default:
                sendMsg("Could not list messages: Unidentified Error");
                break;
            }
        } else if (strncmp(msg[0], "read", 4) == 0) {
            char text[512];
            switch (readMsg(text)) {
            case 0:
                sendMsg(text);
                break;

            case -1:
                sendMsg("Could not read message: Invalid user!");
                break;

            case -2:
                sendMsg("Could not read message: User does not exist!");
                break;

            case -3:
                sendMsg("Could not read message: Message does not exist!");
                break;

            case -4:
                sendMsg("Could not read message: Invalid message!");
                break;

            default:
                sendMsg("Could not read message: Unidentified Error");
                break;
            }
        } else if (strncmp(msg[0], "del", 3) == 0) {
            switch (delMsg()) {
            case 0:
                sendMsg("Message deleted!");
                break;

            case -1:
                sendMsg("Could not delete message: Invalid user!");
                break;

            case -2:
                sendMsg("Could not delete message: Invalid message!");
                break;

            case -3:
                sendMsg("Could not delete message: Message does not exist!");
                break;

            default:
                sendMsg("Could not delete message: Unidentified Error");
                break;
            }
        } else if (strncmp(msg[0], "quit", 4) == 0) {
            active = false;
            sendMsg("Quitting...");
        } else {
            sendMsg("Not a command!");
        }

        for (int i = 0; i < 64; ++i)
            memset(&msg[i], 0, 1024);

        return 0;
    }

    /*
    * Just dont mind this piece of code...
    */
    void formatBuffer() {
        for (int i = 0; i < sizeof(buffer); ++i)
            if (buffer[i] == '^')
                buffer[i] = ' ';
            else if (buffer[i] == ' ')
                buffer[i] = '^';
    }

    /*
    * Strips the Message lines from their \n at the end.
    */
    void formatMsg() {
        for (int i = 0; i < 64; ++i) {
            msg[i][strcspn(msg[i], "\n")] = 0;
            if (strncmp(msg[i], ".", 1) == 0) {
                break;
            }
        }
    }

    /*
    * Writes a message into a file
    *
    * Returns:
    *   0 if Success
    *   !0 if failure (See Client Doc for detailed information)
    */
    int write() {
        if (strncmp(msg[1], ".", 1) == 0)
            return -1;
        else if (strncmp(msg[2], ".", 1) == 0)
            return -2;
        else if (strncmp(msg[3], ".", 1) == 0)
            return -3;
        //else if (strlen(msg[1]) > 8)
            //return -4;

        return fh->writeFile(msg);
    }

    /*
    * Lists messages from a user
    *
    * Params:
    *   f -> Out Parameter for the list of messages.
    *
    * Returns:
    *   0 -> if Success
    *   !0 -> if failure (See Client Doc for detailed information)
    */
    int listF(char* f) {
        if (strncmp(msg[1], ".", 1) == 0)
            return -1;

        char files[512];

        int e = fh->listFiles(msg[1], files);

        if (e == 0)
            for (int i = 0; i < 512; ++i)
                f[i] = files[i];
        else if (e == -5)
            return -5;
        else
            return -2;

        return 0;
    }

    /*
    * Reads a message from a file
    *
    * Params:
    *   t -> Out Parameter for the text of the message.
    *
    * Returns:
    *   0 if Success
    *   !0 if failure (See Client Doc for detailed information)
    */
    int readMsg(char* t) {
        if (strncmp(msg[1], ".", 1) == 0)
            return -1;
        if (strncmp(msg[2], ".", 1) == 0)
            return -4;

        char text[512];

        if (fh->readFile(msg[1], msg[2], text) == 0)
            for (int i = 0; i < 512; ++i)
                t[i] = text[i];
        else
            return -3;

        return 0;
    }

    /*
    * Delets a message from a user
    *
    * Returns:
    *   0 if Success
    *   !0 if failure (See Client Doc for detailed information)
    */
    int delMsg() {
        if (strncmp(msg[1], ".", 1) == 0)
            return -1;
        if (strncmp(msg[2], ".", 1) == 0)
            return -2;

        return fh->delFile(msg[1], msg[2]);
    }
};