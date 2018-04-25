/** client.c - The client side of the socket 
  *
  * @author     Will Roberts, Jacob Vangore
  * @id         1734698,1721023
  * @course     CSIS-380-01/CSIS-480-01
  * @assignment     lab11
*/
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>   /* For AFUNIX sockets */
#include "/usr/include/netinet/in.h"
#include "/usr/include/arpa/inet.h"
#include "/usr/include/netdb.h"

#define MAX 2000
#define DEFAULT_PROTOCOL 0 

/** Main driver
  * @return 0 to signify the program's end
*/
int main () {
	int clientFd, serverLen, result, port, client2;
	struct sockaddr_in serverINETAddress;
	char server_reply[200];
	char server_reply2[200];
	struct sockaddr* serverSockAddrPtr;
	serverSockAddrPtr = (struct sockaddr*) &serverINETAddress;
	serverLen = sizeof (serverINETAddress);
	char message[] = "ls";
	port = 8888;
	
	/* Create an INET socket, bidirectional, default protocol */
	clientFd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
	serverINETAddress.sin_family = AF_INET; /* Server domain */
	serverINETAddress.sin_addr.s_addr = htonl (INADDR_ANY);
    	serverINETAddress.sin_port = htons (port);

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(clientFd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	do /* Loop until a connection is made with the server */
	{
		result = connect (clientFd, serverSockAddrPtr, serverLen);
		if (result == -1) {
			sleep (1);
		}/* Wait and then try again */
	} while (result == -1);
	while (1) {
        	printf("%s ", "lilbash $");
            fgets(message, MAX, stdin);
            message[strcspn(message, "\n")] = 0;
			if (strcmp(message, "exit") == 0) {
				break;
			}
            if( send(clientFd , message , strlen(message) , 0) < 0) {
                puts("Send failed");
                return 1;
            }
		
            memset(message, 0, strlen(message));
			
			if(recv(clientFd, server_reply2 , 2000 , 0) < 0) {
                fprintf(stderr, "%s\n", "Recieve failed");
			}
			printf("%s", server_reply2);
			memset(server_reply2, 0, strlen(server_reply2));
	}
	return 0; /* Done */
}
