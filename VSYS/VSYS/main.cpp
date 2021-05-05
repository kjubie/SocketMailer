/*
@Author: Alexander Bruckner
@Contact: 0664 1642160 (Falls es fragen gibt bei der Bewertung)

Tutorial:
    - Setup: 
        Go to FileHandler.cpp and replace #define STDPATH "/home/abru/mail" with your desired path.

    - Start Client (Should be started after Server!):
        ./VSYS.out 5252
                     ^
                     |
                    PORT

    Login is not yet implemented, therefore anyone can read anything (for now)! Users and their folders for their messages are only created after a message has been sent to a user!
*/

#include <stdio.h>
#include "Server.cpp"

int main(int argc, char** argv) {
	Server* s = new Server(argv[1]);
	s->createSocket();
	delete(s);
	return 0;
}