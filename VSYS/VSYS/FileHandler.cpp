#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <mutex>

#define STDPATH "/home/abru/mail"

using namespace std;

static mutex mtx;

class FileHandler {
private:
	FILE* fptr;

public:
	FileHandler() {

	}

	/*
	* Writes a message from a file
	* Filename -> <header>_<count> e.g.: test_0
	*				if header already exists then increase count e.g.:	test_0
	*																	test_1
	*																	msg_0
	*																	test_2
	*																	hallo_0
	*																	msg_1
	*
	* Params:
	*   msg -> Message sent from Client
	*
	* Returns:
	*   0 if Success
	*   !0 if failure (See Client Doc for detailed information)
	*/
	int writeFile(char msg[64][1024]) {
		char dirPath[64], filePath[64], line[64], *ch, recip[16][8];
		int count = 0, success = 0, countR = 0;

		ch = strtok(msg[1], " ");

		while (ch != NULL) {
			strcpy(recip[countR], ch);
			ch = strtok(NULL, " ");
			++countR;
		}

		for (int rec = 0; rec < countR; ++rec)
			if (strlen(recip[rec]) > 8) {
				return -4;
			}

		for (int rec = 0; rec < countR; ++rec) {
			do {
				mtx.lock();

				sprintf(dirPath, "%s/%s", STDPATH, recip[rec]); //Format Path

				sprintf(filePath, "%s/%s_%d", dirPath, msg[2], count); //Append filename to Path (STDPATH/<user>/<header>_<count>)

				struct stat st = { 0 };

				if (stat(dirPath, &st) == -1) {
					mkdir(dirPath, S_IRWXU | S_IRWXG | S_IRWXO);	//Create user dir if not existing
				}

				fptr = fopen(filePath, "r"); //Test if file already exists

				if (fptr == 0) { //If file does not exist
					success = 1;
					fptr = fopen(filePath, "w"); //Create and open file
					for (int i = 3; i < 64; ++i) {
						sprintf(line, "%s\n", msg[i]);
						if (strncmp(msg[i], ".\n", 1) != 0) //Write line if line != "." (Which marks end of message)
							fputs(line, fptr);
					}
				} else //If file already exists, increase count and try again until filename is free.
					++count;

				fclose(fptr);
				mtx.unlock();
			} while (!success);

			count = 0;
		}

		return 0;
	}

	/*
	* Reads a message from a file
	*
	* Params:
	*   user -> User of message
	*   file -> Filename of massege
	*	text -> Out parameter for message read
	*
	* Returns:
	*   0 if Success
	*   !0 if failure (See Client Doc for detailed information)
	*/
	int readFile(char* user, char* file, char* text) {
		char filePath[64], msg[512] = "", line[512];

		sprintf(filePath, "%s/%s/%s", STDPATH, user, file); //Format Path

		fptr = fopen(filePath, "r"); //Open file

		if (fptr != 0) {
			while (fgets(line, 512, fptr)) { //Read lines into line
				line[strcspn(line, "\n\r")] = '\0'; //Replace line ending with string ending
				if (strncmp(line, "", 1) != 0) //Only save not empty lines into msg
					sprintf(msg, "%s\n%s", msg, line);
			}

			strcpy(text, msg);
			
		} else {
			fclose(fptr);
			return -1;
		}

		fclose(fptr);

		return 0;
	}

	/*
	* Lists messages from a user
	*
	* Params:
	*   user -> User of message
	*	listedFiles -> Out parameter for messages
	*
	* Returns:
	*   0 if Success
	*   !0 if failure (See Client Doc for detailed information)
	*/
	int listFiles(char* user, char* listedFiles) {
		DIR* dir;
		struct dirent* ent;
		char dirPath[64], files[512] = "";
		int count = 0;

		sprintf(dirPath, "%s/%s", STDPATH, user); //Format Path

		if ((dir = opendir(dirPath)) != NULL) { //Open dir
			while ((ent = readdir(dir)) != NULL) { //List files
				if (strncmp(ent->d_name, ".", 1) != 0 && strncmp(ent->d_name, "..", 2) != 0) { //Dont save "." and ".." file into files
					sprintf(files, "%s\n%s", files, ent->d_name);
					++count;
				}
			}

			closedir(dir);

			if (count == 0) {
				delete(files, dirPath);
				return -5;
			}

			strcpy(listedFiles, files);
			
			return 0;
		} else {
			return -1;
		}
	}

	/*
	* Deletes a message from a user
	*
	* Params:
	*   user -> User of message
	*	file -> Message filename to delete
	*
	* Returns:
	*   0 if Success
	*   !0 if failure (See Client Doc for detailed information)
	*/
	int delFile(char* user, char* file) {
		mtx.lock();

		char filePath[64];

		sprintf(filePath, "%s/%s/%s", STDPATH, user, file); //Format Path

		if (remove(filePath) == 0) //Remove file
			return 0;
		else
			return -3;

		return -4;

		mtx.unlock();
	}
};