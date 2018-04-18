/** Chef.c - The server side of the socket 
  *
  * @author     Will Roberts, Jacob Vangore
  * @id         1734698,1721023
  * @course     CSIS-380-01/CSIS-480-01
  * @assignment     lab10
*/

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>      /* For AFUNIX sockets */
#include "/usr/include/netinet/in.h"
#include "/usr/include/arpa/inet.h"
#include "/usr/include/netdb.h"

#define MAX 2000
#define DEFAULT_PROTOCOL 0
/**************************************************************/

/** Main driver
  * @return 0 to signify the program's end
*/
main () {
	int serverFd, clientFd, serverLen, clientLen, client2, port;
	struct sockaddr_in serverINETAddress; /* Server address */
	struct sockaddr_in clientINETAddress; /* Client address */
	struct sockaddr* serverSockAddrPtr; /* Ptr to server address */
	struct sockaddr* clientSockAddrPtr; /* Ptr to client address */
	char server_reply[2000];
	char *message;
	port = 1763;
        
	/* Ignore death-of-child signals to prevent zombies */
	signal (SIGCHLD, SIG_IGN);
	
	serverSockAddrPtr = (struct sockaddr*) &serverINETAddress;
	serverLen = sizeof (serverINETAddress);
	
	clientSockAddrPtr = (struct sockaddr*) &clientINETAddress;
	clientLen = sizeof (clientINETAddress);
	
	/* Create a UNIX socket, bidirectional, default protocol */
	serverFd = socket (AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
	serverINETAddress.sin_family = AF_INET; /* Set domain type */
    	serverINETAddress.sin_addr.s_addr = htonl (INADDR_ANY);
    	serverINETAddress.sin_port = htons (port);

	//strcpy (serverINETAddress.sun_path, "recipe"); /* Set name */
	unlink ("recipe"); /* Remove file if it already exists */
	bind (serverFd, serverSockAddrPtr, serverLen); /* Create file */

	listen (serverFd, 5); /* Maximum pending connection length */
    
	clientFd = accept (serverFd, clientSockAddrPtr, &clientLen);
	while (1) /* Loop forever */
	{
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

	}
	
	close(clientFd);
}

/****************************************************************/

writeRecipe (fd)
int fd;
{
	static char* line1 = "spam, spam, spam, spam,";
	static char* line2 = "spam, and spam.";
	write (fd, line1, strlen (line1) + 1); /* Write first line */
	write (fd, line2, strlen (line2) + 1); /* Write second line */
}

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


readRecipe (fd)
int fd;
{
    char str[200];
    while (readLine (fd, str)) /* Read lines until end-of-input */
        printf ("%s\n", str); /* Echo line from socket */
}


