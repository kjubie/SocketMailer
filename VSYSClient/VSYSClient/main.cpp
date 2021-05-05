/*
@Author: Alexander Bruckner
@Contact: 0664 1642160 (Falls es fragen gibt bei der Bewertung)

Tutorial:
    - Start Client (Should be started after Server!):
        ./VSYSClient.out 127.0.0.1 5252
                            ^       ^
                            |       |
                            IP     PORT

    Login is not yet implemented, therefore anyone can read anything (for now)! Users and their folders for their messages are only created after a message has been sent to a user!


    Warning: ALL Commands are case sensitive!

    Press Enter after each line!

    - SEND Message:
        send                <- Command
        username            <- User to send to
        header              <- Header of Message
        nachricht           <- Text of Message (max 64 lines)
        nachricht
        ..
        ..
        ..
        nachricht
        .                   <- End of Command

        Returns:
            Message sent!                                               <- On Success

            Could not send message You must provide a user!             <- When Command:
                                                                            send
                                                                            .

            Could not send message: You must provide a header!          <- When Command:
                                                                            send
                                                                            user
                                                                            .

            Could not send message: You must provide a text!            <- When Command:
                                                                            send
                                                                            user
                                                                            header
                                                                            .

            Could not send message: Unidentified Error                  <- If this occurs pls contact me^^

    - READ Message:
        read                <- Command
        username            <- User to read from
        filename            <- Filename of Message (Usually <header>_0)
        .                   <- End of Command

        Returns:
            Message                                                     <- On Success

            Could not read message: Invalid user!                       <- When Command:
                                                                            read
                                                                            .

            Could not read message: Invalid message!                    <- When Command:
                                                                            read
                                                                            filename
                                                                            .

            Could not read message: Message does not exist!             <- When user or message do not exist

            Could not read message: Unidentified Error                  <- If this occurs pls contact me^^

    - LIST Messages:
        list                <- Command
        username            <- User to list messages from
        .                   <- End of Command

        Returns:
            List of message files                                       <- On Success

            Could not list files: Invalid user!                         <- When Command:
                                                                            list
                                                                            .

            Could not list files: User does not exist!                  <- When user does not exist

            Could not list files: User has no Messages!                 <- When user has an empty message dir

            Could not list messages: Unidentified Error                 <- If this occurs pls contact me^^

    - DELETE Message:
        del                 <- Command
        username            <- User to list messages from
        filename            <- Filename of Message (Usually <header>_0)
        .                   <- End of Command

        Returns:
            Message deleted!                                            <- On Success

            Could not delete message: Invalid user!                     <- When Command:
                                                                            del
                                                                            .

            Could not delete message: Invalid message!                  <- When Command:
                                                                            del
                                                                            user
                                                                            .

            Could not delete message: Message does not exist!           <- When user or message do not exist

            Could not delete message: Unidentified Error                  <- If this occurs pls contact me^^

    - QUIT:
        quit                <- Command
        .                   <- End of Command
*/

#include <stdio.h>
#include "Client.cpp"

#define BUF 1024
#define PORT 6543

using namespace std;

int main(int argc, char** argv) {
	Client* c = new Client(argv[1], argv[2]);
	c->connectToServer();
	
	delete(c);
	return 0;
}