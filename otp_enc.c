/*******************************************************************
* File: otp_enc Program 4
* Author: Chelsea Starr
* Date: 11/30/2018
* Desctiption: gets plaintext, key, and port from user and outputs
* ciphertext to stdout using OTP encryption.
* - otp_enc(client) connects to otp_enc_d(host) via the specified port
* - command line syntax: otp_enc plaintext key port
* - client.c from class used as skeleton
*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#define MAX_MSG_SIZE 256

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues
char* read_file(char*);
int check_plainT(char*);
void my_itoa(int, char*);

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead, flag;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[MAX_MSG_SIZE];
	char Kbuffer[MAX_MSG_SIZE];
	char mssg_len[5];

	if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress));  // Clear out the address struct
	portNumber = atoi(argv[3]);                                  // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET;                          // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber);                  // Store the port number
	serverHostInfo = gethostbyname("localhost");                  // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0);                  // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	// Get plaintext from file and add
	memset(buffer, '\0', sizeof(buffer));        // Clear out the buffer array
	strcpy(buffer, read_file(argv[1]));
	buffer[strcspn(buffer, "\n")] = '\0';        // Remove the trailing \n that fgets adds

	if(check_plainT(buffer) == 0) { return 0; }

	// Send plaintext to server
	// printf("sending plaintext...\n");
	charsWritten = send(socketFD, buffer, MAX_MSG_SIZE, 0);    // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");


// ENSURE KEY AND INPUT TEXT ARE VALID
	// Get key from file
	memset(Kbuffer, '\0', sizeof(Kbuffer));        // Clear out the buffer array
	strcpy(Kbuffer, read_file(argv[2]));
	Kbuffer[strcspn(Kbuffer, "\n")] = '\0';        // Remove the trailing \n that fgets adds

	if(strlen(Kbuffer) < strlen(buffer)) { fprintf(stderr, "CLIENT: ERROR: key too short\n"); exit(0);}

	// Send key to server
	charsWritten = send(socketFD, Kbuffer, strlen(Kbuffer), 0);    // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(Kbuffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return cipher from server
	memset(buffer, '\0', sizeof(buffer));                        // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);   // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	printf("%s\n", buffer);

	close(socketFD); // Close the socket
	return 0;
}

// FIX: CHECK THAT ALL CHARS ARE VALID!!!
char* read_file(char* file_name)
{
	int fd;
	char* mssg = (char *)malloc(100000* sizeof(char));
	// char mssg[MAX_MSG_SIZE];

	fd = open(file_name, O_RDONLY);
	if (fd == -1)
  {
    fprintf(stderr, "open() failed");
    exit(1);
  }

	read(fd, mssg, MAX_MSG_SIZE);

	return mssg;
}


void my_itoa(int n, char s[])
{
    int i, len, sign = n;
//
    if (n < 0)
        n = -n;
    i = 0;
    do
    {
        s[i++] = n % 10 + '0';
    } while ( (n /= 10) > 0);

    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';

		len = strlen(s);
		for(i=0;i<strlen(s)/2;i++)
   {
         s[i]+=s[len];
         s[len]=s[i]-s[len];
         s[i]=s[i]-s[len--];
   }
}

int check_plainT(char* mssg)
{
	int i;
	for(i = 0; i < strlen(mssg); i++)
	{
		if( ((mssg[i] < 'A') || (mssg[i] > 'Z')) && (mssg[i] != ' ') )
		{
			fprintf(stderr, "bad input file\n");
			return 0;
		}
	}
	// mssg[strlen(mssg)] = '#';
	return 1;
}

//FIX: Make MAX_MSG_SIZE variable becuase some files are BIG!
//FIX: make so multiple connections can be made to server at once (with forking? select()?)
