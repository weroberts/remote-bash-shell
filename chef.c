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
#include <string.h>
#include <sys/types.h>

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
	port = 8888;

	
    char line[MAX];

    char *argsLeft[MAX];
    char *argsRight[MAX];
    char commandsLeft[MAX];
    char commandsRight[MAX];
	//-------------------------------------------------------------
	char prompt[] = "shell $";        
	/* Ignore death-of-child signals to prevent zombies */
	signal (SIGCHLD, SIG_IGN);
		char str[200];
	
	serverSockAddrPtr = (struct sockaddr*) &serverINETAddress;
	serverLen = sizeof (serverINETAddress);
	
	clientSockAddrPtr = (struct sockaddr*) &clientINETAddress;
	clientLen = sizeof (clientINETAddress);

	int outputFd;
	int fp;
	int outputCopy;
	char *buffer =  malloc(MAX);
	char template[] = "/tmp/myFileXXXXXX";
	int savedStdout;
		outputFd = mkstemp(template);
	//close(savedStdout);
	//close(outputFd);
	//savedStdout = du/p(1);
	
	/* Create a UNIX socket, bidirectional, default protocol */
	serverFd = socket (AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
	serverINETAddress.sin_family = AF_INET; /* Set domain type */
    	serverINETAddress.sin_addr.s_addr = htonl (INADDR_ANY);
    	serverINETAddress.sin_port = htons (port);
	//char str[200];

	int enable = 1;
	int count = 0;
	setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	//strcpy (serverINETAddress.sun_path, "recipe"); /* Set name */
	unlink ("recipe"); /* Remove file if it already exists */
	bind (serverFd, serverSockAddrPtr, serverLen); /* Create file */

	listen (serverFd, 5); /* Maximum pending connection length */
    
	clientFd = accept (serverFd, clientSockAddrPtr, &clientLen);
	while (1) {
		//fp = fdopen(outputFd, "r+");
		if (fp == NULL) {
			fprintf(stderr, "%s\n", "Failed to open fp!");
		}
		/* print prompt */
		memset(line, 0, strlen(line));
        /* fill line with raw input from the user */
		if(recv(clientFd, line, 2000 , 0) < 0) {
        	puts("Recieve failed");
        }
        if (parsePipes(line, commandsLeft, commandsRight) == 0) {
			parseArguments(commandsLeft, argsLeft);
			if(strcmp(argsLeft[0], "exit") == 0) {
        		break;
        	}
			//close(1); // remove stdout from slot1
			//dup2(outputFd, 1); // set outputFd to slot 1	
				
			savedStdout = dup(1); //save stdout
			dup2(outputFd, 1);
        	executeOne(argsLeft, outputFd);
			// return to stdout
			dup2(savedStdout, 1);
			close(savedStdout);
        } else {
            parseArguments(commandsLeft, argsLeft);
            parseArguments(commandsRight, argsRight);
			if((strcmp(argsRight[0], "exit") == 0) || (strcmp(argsLeft[0], "exit") == 0)) {
                break;
            }

            executeTwo(argsLeft, argsRight);
		}
		lseek (outputFd, (off_t) 0, SEEK_SET);
		//sendFile(outputFd);
		//strcpy(str, "");
    	readLine (outputFd, str); /* Read lines until end-of-input */
		//if (fclose(fp) != 0) {
		//	fprintf(stderr, "%s\n", "Error closing file pointer!");
		//}
		fprintf(stderr, "%s\n", str);
            if( send(clientFd , str , strlen(str) , 0) < 0)
            {
                puts("Send failed");
                return 1;
            }

		memset(str, 0, strlen(str));
		*str = '\0';
		//readRecipe(outputFd);
		//close(outputFd);
		//fp = fdopen(outputFd);
		ftruncate(outputFd, 0);
		lseek (outputFd, (off_t) 0, SEEK_SET);
	}
	close(outputFd);
	close(clientFd);
}

/****************************************************************/

readLine (int fd, char *str)
/* Read a single NULL-terminated line */
{
    int n;
    do /* Read characters until NULL or end-of-input */
    {
        n = read (fd, str, 1); /* Read one character */
    }
    while ((n > 0) && *str++ != '\0');
    return (n > 0); /* Return false if end-of-input */
}

readRecipe (fd)
int fd;
{
    char str[200];
    printf(stderr, "%s\n", str); /* Echo line from socket */
}


/*sendFile (fd, sendingFd)
int fd;
int sendingFd;
{
    char str[200];
    while (readLine (fd)) 
        if (send(sendingFd, str, strlen(str), 0) < 0) {
			fprintf(stdout, "send failed");
		}
}*/

/** Parse the inputString into an 2 arrays separated by pipes
*   @param **inputString String of input
*   @param **commandsLeft Commands on the left of the pipe
*   @param **commandsRight Commands on the right of the pipe
*/
int parsePipes(char *inputString, char *commandsLeft, char *commandsRight) {
    if (strchr(inputString, '|') == NULL) {
    	strcpy(commandsLeft, inputString);
        return 0;
    }
    char *token;
    token = strtok(inputString, "|");
    strcpy(commandsLeft, token);
    token = strtok(NULL, "|");
    strcpy(commandsRight, token);
    return 1;
}

/** Parse the inputString into an array, separated by spaces
*   @param **inputString String of input
*   @param **argumentsArray Array of arguments
*/
void parseArguments(char *inputString, char **argumentsArray) {
    char *token;
    token = strtok(inputString, "\n");
    token = strtok(token, " ");
    int x = 0;
    while (token != NULL) {
    	argumentsArray[x] = token;
        x++;
        token = strtok(NULL, " ");
    }
    argumentsArray[x] = NULL;
}

/** Executes "argsLeft" and "argsRight" through execvp in child process 
*   @param **argsLeft array of strings that make up a command on the left of a pipe
*   @param **argsRight array of strings that make up a command on the right of a pipe
*/
void executeTwo(char **argsLeft, char **argsRight)
{
    int fd[2]; 
    pid_t p1, p2;
 
    pipe(fd);
    p1 = fork();

    if (p1 == 0) {
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);
        execvp(*argsLeft, argsLeft); 
    } 
    else {

        p2 = fork();

        if (p2 == 0) {
            close(fd[1]);
            dup2(fd[0], 0);
            close(fd[0]);
            execvp(*argsRight, argsRight);
        } else {
            close(fd[1]);
            close(fd[0]);
            wait(NULL);
            wait(NULL);
        }
    }
}
     
/** Executes "command" through execvp in child process 
*   @param **command array of strings that make up a command 
*/
void executeOne(char **command) {

    int status;
    pid_t pid;
    pid_t c;

    pid = fork();
    if(pid == 0) {
        execvp(*command, command);
        /*statement should not execute */
        fprintf(stderr, "Failed to execute command\n");
		return;
    }
    else if(pid < 0) {
        //fork failed
        fprintf(stderr, "Failed to fork\n");
		return;
    }
    else {
        //wait for child
        c = wait(&status);
        if (c != pid) {
			printf("%s", "Error!");
            c = wait(&status);
        }
    }
}

