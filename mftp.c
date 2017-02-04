//Tyler Crabtree
//Minature File Transfer Program (mftp)
//WSU Vancouver


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "mftp.h"
// This global int helps display message/prompt correctly. 
#define MY_PORT_NUMBER 49999
int globalCheck = FALSE;




////////////////////////////////////////////
//helperFunction() this is and addional function that prints possible commands 
//////////////////////////////////////////// 
void helperFunction(){
	printf("Command list:\n");
	printf("exit\n");
	printf("rcd <pathname>\n");
	printf("ls\n");
	printf("rls\n");
	printf("get <pathname>\n");
	printf("put <pathname>\n");

}


////////////////////////////////////////////
//grabMessage() manipulates and returns buffer
//////////////////////////////////////////// 
char* grabMessage(int socketfd){
	char* buffer = malloc(TEXT);
	register int i;
	char check;
	read(socketfd, &check, TRUE);
	for(i = NORMAL; check != '\n'; i++){
		buffer[i] = check;
		read(socketfd, &check, TRUE);
	}
	buffer[i] = '\0';
	return buffer;
}

////////////////////////////////////////////
//setup() this functions checks the connection and socket; it produces error messages if needed
//////////////////////////////////////////// 
int setup(char* serverID, int port){
	// Much of this code (within this function) is supplied by Dr. Lang 
	struct sockaddr_in servAddr;
	struct hostent* hostEntry;
	struct in_addr **pptr;

	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0) {
	    fprintf(stderr, "ERROR!\n Socket could not be opened. %s\n", strerror(errno));
	    return FALSE;
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);

	hostEntry = gethostbyname(serverID);
	if (!hostEntry){
	    fprintf(stderr, "ERROR!\n Host name could not be used. %s\n" , strerror(errno));
		return FALSE;
	}
	pptr = (struct in_addr **) hostEntry->h_addr_list;
	memcpy(&servAddr.sin_addr, *pptr, sizeof(struct in_addr));

	if (connect(socketfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0){
		fprintf(stderr, "ERROR!\n Connection could not be made. %s\n", strerror(errno));
		return FALSE;
	}
	return socketfd;  //socket
}


////////////////////////////////////////////
//EXIT() exits the program (all caps to avoid built in function)
//////////////////////////////////////////// 
void EXIT(){
	printf("Disconnected properly.\n");
	exit(0);
}

////////////////////////////////////////////
//ls() lists the client's directory
//////////////////////////////////////////// 
void ls(){
	int first = TRUE;
	if((first = fork())){
		wait(&first);
		// format (wanted it clear, and to seperate other commands)
		printf("\n");
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		printf("		COMMAND COMPLETED\n");
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		printf("\n");	

	} else {
		int fd[2];
		pipe(fd);
		int second = TRUE;
		int rd = fd[0];
		int wt = fd[1];
		if((second = fork())){
			close(wt);
			close(0); 
			dup(rd);
			close(rd);
			wait(&second);
			execlp("more", "more", "-20", NULL);				
		} else {
			close(rd);
			close(1);
			dup(wt);
			close(wt);
			execlp("ls", "ls", "-l", NULL); 
		}
	}
}



////////////////////////////////////////////
//rls() produces an error if cd cannot be executed
//////////////////////////////////////////// 
void rls(int fd){
	int first = TRUE;
	if((first = fork())){
		wait(&first);
		printf("\n");
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		printf("		COMMAND COMPLETED\n");
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		printf("\n");
	} else {
		close(0);
		dup(fd);
		execlp("more", "more", "-20", NULL);		
	}
}


////////////////////////////////////////////
//cd() produces an error if cd cannot be executed
//////////////////////////////////////////// 
void cd(char* path){
	int changeValue = chdir(path);
	if(0 > changeValue) {
	    fprintf(stderr, "ERROR!\nCD command failed, failed path.\n");// %s\n", strerror(errno));
	}
	else{
		printf("\n");
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		printf("		COMMAND COMPLETED\n");
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		printf("\n");

	}

}





////////////////////////////////////////////
//get() gets file from server
//////////////////////////////////////////// 
void get(int datafd, char* path){

	//time_t start = time(NULL); // These are my time variables.
    //time_t endwait = start + secondsToWait; // this is basically for the counter for the while
	int fd;
	int readIn;
	char buffer[TEXT];
		if((fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0600)) < 0) {
			if( globalCheck == FALSE){
			fprintf(stderr, "Get function error, file exists!\n");
				}
			return;
		}
	
	globalCheck = FALSE;
	while ((readIn = read(datafd, buffer, TEXT))){
		//if(start <= endwait){
		//	printf("timeout error, the file most likely does not getget.");
		//	break;
		//}
		//start = time(NULL);
		write(fd, buffer, readIn);
		}
	close(fd);
	printf("\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("		COMMAND COMPLETED\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\n");
	return;
}




////////////////////////////////////////////
//put() manipulates and returns buffer
//////////////////////////////////////////// 
int put(int datafd, int socketfd, char* word, char* path){
	// defining needed variables for open/write
	int fd;
	int openRead;
	char* buffer;
	char* newBuffer;

		if((fd = open(path, O_RDONLY, 0600)) < 0) {
	   		fprintf(stderr, "ERROR!\nPut error. \n"); //, strerror(errno));
			return FALSE;
		}
		write(socketfd, word, strlen(word));
		buffer = grabMessage(socketfd);
		if (buffer[0] == 'E'){
			newBuffer = strcat(buffer+1, "\0");
			printf("%s\n", newBuffer );
			return FALSE;
		} else {
			while ((openRead = read(fd, buffer, TEXT)) > 0){
				write(datafd, buffer, openRead);
			}
		}
	close(fd);
	//I wanted clear and consistent success prompts
	printf("\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("		COMMAND COMPLETED\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("\n");
	return 0;
}

int main(int argc, char** argv){

	char* buffer;
	char* CC = NULL;  // accronym for client command
	char SC[TEXT];// accronym for sever command
	char *path;
	char input[TEXT];
	int datafd = 0;

	// I tried to make this code very readable. If the first argument is not (or not true), produce an error. 
	if(!argv[TRUE]){  
	    fprintf(stderr, "ERROR!\n Please enter a valid hostname. %s\n", strerror(errno));
		return FALSE;
	}

	char* serverID = argv[1];
	int socketfd = setup(serverID, MY_PORT_NUMBER);
	
	if (socketfd < 0) {
		return FALSE;
	}	
 	int firstHelp = 0;
	while(TRUE){
		label: 
		if (firstHelp != 0){
		printf("Enter a command: "); 
		}

		if (firstHelp == 0){
		printf("Enter a command or type 'help' to see command list: ");
		firstHelp++; 
		}

		fgets(input, TEXT, stdin);
		printf("\n");

		if (input[0] == EOF){
			break;
		}
		if (input[0] == '\n'){
			goto label;
		}
	
		printf("%s", input);
		CC = strtok(input, " \n\t\v\f\r");
		if (!strcmp(CC, "exit") ) {
			write(socketfd, "Q\n", 2);
			buffer = grabMessage(socketfd);
			if(buffer[0] == 'A'){
			printf("Disconnected from server normally.\n");
			return 0;
			}}

		//This is where I implement the second page of the prompt. 


		// ADDITIONAL HELP COMMAND
		if (strcmp(CC, "help") == NORMAL){
			helperFunction();
			//fflush(stdout);
			} 
		// EXIT COMMAND
		//else if (strcmp(CC, "exit") == 0) {
		//	write(socketfd, "Q\n", 3);  // Reply with Q command
		//	EXIT();
		//} 

		//CD, calls functions, then flushes
		else if (strcmp(CC, "cd") == NORMAL){
			path = strtok(NULL, " \n\t\v\f\r");
			cd(path);
			fflush(stdout);


		//LS,  calls functions, then flushes
		} else if (strcmp(CC, "ls") == NORMAL){
			ls();
			fflush(stdout);

		//rcd interacts with server,
		} else if (strcmp(CC, "rcd") == NORMAL){
			fflush(stdout);
			path = strtok(NULL, " \n\t\v\f\r");
		    if(!path){
		    	printf("Error, path does not exist.\n");
		    	goto label;
		    }
			strcpy(SC, "C");
			strcat(SC, path);
			strcat(SC, "\n");	
			printf("%s", SC);
			int length = strlen(SC);
			write(socketfd, SC, length);
			buffer = grabMessage(socketfd);
			printf("%s", buffer);
			fflush(stdout);
			if (buffer[0] == 'E'){
				printf("%s\n", strcat(buffer+1, "\n"));
			}


		//rls command
		} else if (strcmp(CC, "rls") == NORMAL){

			write(socketfd, "D\n", replyLength);
			buffer = grabMessage(socketfd);
			fflush(stdout);
			assert(buffer);
			if (buffer[0] == 'E'){
				printf("%s\n", strcat(buffer+1, "\n"));
			} else {
				datafd = setup(serverID, atoi(buffer+1));
				if (datafd == FALSE){  // ensures this is a correct value
				return FALSE;          // if not, exit
				}
				write(socketfd, "L\n", replyLength);
				buffer = grabMessage(socketfd);
				assert(buffer);
				if(buffer[0] == 'A'){
					assert(datafd);  // double check
					rls(datafd);
				} else {
					printf("Error: %s\n", strcat(buffer+1, "\n"));
				}
		}


		//get command
		} else if (strcmp(CC, "get") == NORMAL){
			write(socketfd, "D\n", replyLength);
			buffer = grabMessage(socketfd);
			fflush(stdout);
	
			if (buffer[0] == 'E'){
				printf("%s\n", strcat(buffer+1, "\n"));
			} else {
				datafd = setup(serverID, atoi(buffer+1));
				path = strtok(NULL, " \n\t\v\f\r");
					if(!path){
					printf("get error, bad path\n");
					goto label;
				}
				strcpy(SC, "G");
				strcat(SC, path);
				strcat(SC, "\n");	
				write(socketfd, SC, strlen(SC));
				buffer = grabMessage(socketfd);
				if(buffer[0] == 'A'){
					globalCheck = FALSE;
					assert(datafd);
					assert(path);
					get(datafd, path);
					close(datafd);
				} else{
				 printf("Error: %s\n", strtok(buffer+1, "\n"));
				}
			}

		// show
		} else if (strcmp(CC, "show") == NORMAL){
			fflush(stdout);

			write(socketfd, "D\n", replyLength);
			buffer = grabMessage(socketfd);	

			if (buffer[0] == 'E'){
				printf("%s\n", strcat(buffer+1, "\0"));
			} else {
				datafd = setup(serverID, atoi(buffer+1));
				path = strtok(NULL, " \n\t\v\f\r");
				if(!path){
					printf("show error, bad path\n");
					goto label;
				}				
				strcpy(SC, "G");
				strcat(SC, path);
				strcat(SC, "\n");	
				write(socketfd, SC, strlen(SC));				
				buffer = grabMessage(socketfd);			
				if(buffer[0] == 'A'){
				globalCheck = TRUE;				
				get(datafd, path);
				rls(datafd);
				} else {
					printf("Error: %s\n", strtok(buffer+1, "\n"));
				}
			}


		// put command
		} else if (strcmp(CC, "put") == NORMAL){
			printf("%s", buffer+1);
			fflush(stdout);  
			write(socketfd, "D\n", replyLength);
			printf("%s", buffer+1);
			fflush(stdout);
			buffer = grabMessage(socketfd);	
			if (buffer[0] == 'E'){
				printf("%s\n", strcat(buffer+1, "\n"));
			} else {
				
				datafd = setup(serverID, atoi(buffer+1));
				path = strtok(NULL, " \n\t\v\f\r");
				if(!path){
					printf("put error, bad path\n");
					goto label;
				}
				strcpy(SC,  "P");
				strcat(SC,  path);
				strcat(SC,  "\n");	
				put(datafd,  socketfd,  SC,  path);
				close(datafd);
			}
		} else {
	    		fprintf(stderr, "ERROR!\nPlease enter a valid command.\n"); // %s\n"); //, strerror(errno));
		}	

	}
	free(buffer);
	return NORMAL;
}
