#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>   /* For AFUNIX sockets */
#include "/usr/include/netinet/in.h"
#include "/usr/include/arpa/inet.h"
#include "/usr/include/netdb.h"

#define DEFAULT_PROTOCOL 0 

/****************************************************************/
main ()
{
	int clientFd, serverLen, result, port, client2;
	struct sockaddr_in serverINETAddress;
	char *message, server_reply[200];	
	struct sockaddr* serverSockAddrPtr;
	serverSockAddrPtr = (struct sockaddr*) &serverINETAddress;
	serverLen = sizeof (serverINETAddress);

	port = 1763;
	
	/* Create a UNIX socket, bidirectional, default protocol */
	clientFd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
	serverINETAddress.sin_family = AF_INET; /* Server domain */
	serverINETAddress.sin_addr.s_addr = htonl (INADDR_ANY);
    	serverINETAddress.sin_port = htons (port);

	//strcpy (serverINETAddress.sun_path, "recipe"); /* Server name */
	//listen (clientFd, 5); /* Maximum pending connection length */
    	//bind (clientFd, serverSockAddrPtr, serverLen); /* Create file */

	do /* Loop until a connection is made with the server */
	{
		result = connect (clientFd, serverSockAddrPtr, serverLen);
		if (result == -1) {
			sleep (1);
		}/* Wait and then try again */
	} while (result == -1);
	while (1) {
     	memset(server_reply, 0, sizeof(server_reply));
		if(recv(clientFd, server_reply , 2000 , 0) < 0)
    		{
        		puts("recv failed");
    		}
    		puts(server_reply);
		message = "I got it!";
		if(send(clientFd , message , strlen(message) + 1, 0) < 0)
    		{
        		puts("Send failed");
        		return 1;
    		}
		//close(clientFd); /* Close the socket */
	}
	return 0; /* Done */
}
/**************************************************************/
readRecipe (fd)
int fd;
{
	char str[200];
	while (readLine (fd, str)) /* Read lines until end-of-input */
		printf ("%s\n", str); /* Echo line from socket */
}
/**************************************************************/
readLine (fd, str)

int fd;
char* str;
/* Read a single NULL-terminated line */
{
	int n;
	do /* Read characters until NULL or end-of-input */
	{
		n = read (fd,str, 1); /* Read one character */
	}
	while ((n > 0) && *str++ != '\0');
	return (n > 0); /* Return false if end-of-input */
}

writeRecipe (fd)
int fd;
{
    static char* line1 = "Got it.";
    static char* line2 = "They got it.";
    write (fd, line1, strlen (line1) + 1); /* Write first line */
    write (fd, line2, strlen (line2) + 1); /* Write second line */
}

