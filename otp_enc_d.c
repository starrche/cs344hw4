/*******************************************************************
* File: otp_enc_d Program 4
* Author: Chelsea Starr
* Date: 11/30/2018
* Desctiption: "server" performing encryption.
* -Should be run in background (use &)
* -Command line syntax: otp_enc_d listening_port &
* -Waits and listens on specified port. Gets message and key from
* 	user and replies with ciphertext.
* - server.c from class used as skeleton
*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_SIZE 256

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues
char* encrypt(char*, char*);

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, len;
	socklen_t sizeOfClientInfo;
	char buffer[MAX_SIZE], Kbuffer[MAX_SIZE];
	struct sockaddr_in serverAddress, clientAddress;
	char cipher[MAX_SIZE];

	pid_t spawnPid = -5;
	pid_t allPids[5];
	int childExitStatus = -5;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress));	// Clear out the address struct
	portNumber = atoi(argv[1]);																		// Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET;														// Create a network-capable socket
	serverAddress.sin_port = htons(portNumber);										// Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY;										// Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); 						// Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

  // FIX: implement apparent concurrency (select()) so multiple clients can be serviced at once
  // infinite loop allowing server to continuously listen for clients
  while(1)
  {

		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");


//FIX: Why does strlen(mssg) return incorrectly only when there is forking?

		spawnPid = fork();
		switch (spawnPid)
		{
			case -1:			// error
        perror("Hull Breach!\n");
				exit(1);
        break;

			case 0:				// child

				// printf("SERVER: Connected Client at port %d\n", ntohs(clientAddress.sin_port));

				// get length of plaintext from client
				// memset(buffer, '\0', MAX_SIZE);
		    // charsRead = recv(establishedConnectionFD, buffer, MAX_SIZE, 0); // Read the client's message from the socket
		    // if (charsRead < 0) error("ERROR reading from socket");
				// printf("buffer1: %s\n", buffer);
				// len = atoi(buffer);


		    // Get the plaintext from the client
		    memset(buffer, '\0', MAX_SIZE);
		    charsRead = recv(establishedConnectionFD, buffer, MAX_SIZE, 0); // Read the client's message from the socket
		    if (charsRead < 0) error("ERROR reading from socket");
		    // printf("SERVER: I received this from the client: \"%s\"\n", buffer);

				// get client key
				memset(Kbuffer, '\0', MAX_SIZE);
				charsRead = recv(establishedConnectionFD, Kbuffer, MAX_SIZE, 0); // Read the client's message from the socket
				if (charsRead < 0) error("ERROR reading from socket");
				// printf("SERVER: I received this from the client: \"%s\"\n", Kbuffer);

				// create cipher
				strcpy(cipher,encrypt(buffer, Kbuffer));
				// printf("SERVER: I encrypted this: \"%s\"\n", cipher);

				// Send a cipher back to the client
				charsRead = send(establishedConnectionFD, cipher, strlen(cipher), 0);
				if (charsRead < 0) error("ERROR writing to socket");

				close(establishedConnectionFD); // Close the existing socket which is connected to the client

        exit(0);

			default:			// parent	FIX: CLEAN UP ZOMBIES!
				waitpid(spawnPid, &childExitStatus, WNOHANG);		// block until child process finishes
				break;
		}


  }
	close(establishedConnectionFD); // Close the existing socket which is connected to the client
	close(listenSocketFD); // Close the listening socket
	return 0;
}


char* encrypt(char* mssg, char* key)
{
	char* cipher;
	int i;
	int temp;
	int mssg_int;
	cipher = (char*) malloc(MAX_SIZE * sizeof(char));

	// printf("strlen(mssg): %s\n", strlen(mssg));

	for(i = 0; i < strlen(mssg); i++)
	{
		if(mssg[i] == ' ') { mssg[i] = '[';}
		if(key[i] == ' ') { key[i] = '[';}

		temp = ((mssg[i] + key[i]) - (65 * 2)) % 27;
		cipher[i] = temp + 65;

		if(cipher[i] == '[') { cipher[i] = ' ';}

	}
	return cipher;
}


// FIX: get length of message to be read in (send first?) and then make sure that value is returned by recv() so it is ensured that entire message was read
