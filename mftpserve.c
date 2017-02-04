//Tyler Crabtree
//Minature File Transfer Program (mftp)
//CS 360
//WSU Vancouver



// include sections, most libaries in mftp.h
#include <time.h>
#include "mftp.h"
#define portLength 16
#define writeLength 13
#define MY_PORT_NUMBER 49999 

//#pragma GCC diagnostic push   (for mac)
//#pragma GCC diagnostic ignored "-Wunused-variable"
//Compile without warnings. 

////////////////////////////////////////////
//grabTime() grabs and returns current time
//////////////////////////////////////////// 
char* grabTime(){							// Used function in daytime.c 
	time_t now;  							// Using time.h to find current time 					
	time(&now);								// ""  ""
	return(ctime(&now));					// returns time for connection prompt
}


////////////////////////////////////////////
//readBuffer() reads/manipultes buffer, asserts that buffer isn't null 
//////////////////////////////////////////// 
char* readBuffer(char word, char* buffer , int connectfd ){
	register int i = 0;						// Using register int (learned this year)    	
	while(word != '\n'){            		// while the word isn't at the end.
		buffer[i] = word;					// conversion for read
		i++;								// increments 
		read(connectfd, &word, TRUE);// reads buff	
	}
	buffer[i] = '\0';    // add \0
	assert(buffer);      // extensive testing
	return buffer;		// return buffer in correct format

}

//////////////////////////////////////////// 
// Sense() sets up the listen socket 
//////////////////////////////////////////// 
int sense(int port){
	int fix = TRUE;   						// I added a couple lines of code to ensure the connection could reconnect after it disconnected. 
	int listenfd = socket(AF_INET, SOCK_STREAM, NORMAL);  // From langs slide.
	const socklen_t optimumSize = sizeof(TRUE); 	 // This helps connecting to the server after disconnecting. 
	int errorFix = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (void*) &fix, optimumSize);  // without this, port would close after the first runthrough.
	//printf("%d\n", errorFix );
	return listenfd;
}


//////////////////////////////////////////// 
//creatServer() creates server adress
//////////////////////////////////////////// 
struct sockaddr_in createServer(int port){
    struct sockaddr_in servAddr;			 // This section establishes the server, from Dr. Lang's slides.
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	return servAddr;						// End code from slides.  
}


//////////////////////////////////////////// 
//grabsPort() 
//////////////////////////////////////////// 
char* grabPort(int datafd, struct sockaddr_in servAddr){
	if (bind(datafd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < NORMAL) { // much of this code is provided in dr. lang silides
		perror("bind");
		exit(FALSE);
	}
	char* portno = malloc(portLength); 											 //ERRROR! Doesn't work if portlength is changed (tough fix to find)	

	struct sockaddr_in portSock;
	unsigned int size = sizeof(portSock);
	int test = getsockname(datafd, (struct sockaddr *) &portSock, &size);
    assert(test != FALSE);
    int port = ntohs(portSock.sin_port);
    sprintf(portno, "%d", port);   												// 
	return portno;
}


//////////////////////////////////////////// 
//rcd() changes directory in server
//////////////////////////////////////////// 
int rcd(char* path){
	strtok(path, "\n");   // check path for new line 
	if(chdir(path) == FALSE) {  // if the path doesn't exist, execute an error and return false
		printf( "ERROR!The path does not exist, please try another.\n");  // error
		return FALSE;  // indicate bad exit
	}
	printf("SUCCESS!\n");
	printf("The current directory for the server's terminal has changed.\n");
	return NORMAL;
}


//////////////////////////////////////////// 
//rls() lists server's directory 
////////////////////////////////////////////
void rls(int datafd){
	int check = fork();
	if(!(check)){
		close(1); 
		dup(datafd);
		if(execlp("ls", "ls", "-l" , NULL )){
		fprintf(stderr, "Listing server directory error.\n%s\n", strerror(1));

		exit(FALSE);
		}
	}
	printf("SUCCESS!\n");
	printf("The current directory for the server's terminal has been read.\n");
	wait(&check);
	return;
}

//////////////////////////////////////////// 
//get() gets a file from the server and store it in client's directory
//////////////////////////////////////////// 
void get(int datafd,int connectfd, char* path){
	//strtok(path, "\n");
	int fd;
	int bytes;
	char buf [TEXT];
	if((fd = open(path, O_RDONLY, 0600)) < NORMAL) {
		//fprintf(stderr, "ERROR! The get function failed, please try another\n%s\n", s;
		write(connectfd, "EError! Get failed.\n", 20);
		return;
	}
	write(connectfd, "A\n", replyLength);

	while ((bytes = read(fd, buf, TEXT)) > NORMAL){

		write(datafd, buf, bytes);
	}

	close(fd);
	return;
}


//////////////////////////////////////////// 
//put() puts a file from the clients and store it in server's directory
//////////////////////////////////////////// 


int put(int datafd, int connectfd, char* path){
	//strtok(path, "\n");
	int fd;
	int bytes;
	char buf [TEXT];
		if((fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0600)) < 0) {
			fprintf(stderr, "Put function failed, file already exists\n");
			write(connectfd, "EError! Put Failed.\n", 20);
			return FALSE;
		}
		write(connectfd, "A\n", replyLength);
		while ((bytes = read(datafd, buf, TEXT)) > NORMAL){
			write(fd, buf, bytes);
		}
		close(fd);
	return NORMAL;
}
	

//////////////////////////////////////////// 
//dataConnection () establish data connection
//////////////////////////////////////////// 
int dataConnection(int listenfd){
	struct sockaddr_in clientAddr;
	unsigned int length = sizeof(struct sockaddr_in);
	int connectfd = accept(listenfd, (struct sockaddr *) &clientAddr, &length);
	return connectfd; 
	}

int main(int argc, char** argv){
	char buffferChar;				// For reading/ replying
	char found;						// Used for checking
	char buffer[TEXT];				// Creates buffer with macro
	char reply[TEXT];				// used to reply
	int port = 0;						// Initilizes port number
	int listendatafd = 0;	
	int datafd = 0;		
	char* portGrab;
	printf("Running server...\n");		// Prompts user  that the program is running
	if(argv[1]){  
		int port = atoi(argv[TRUE]); 	// grabs first arg if there is one, else it just assigns my_port_number
	} 
	if(!(argv[1])){						// assign macro to port if none is listed.			
		port = MY_PORT_NUMBER;
	} 

	int listenfd = sense(port); 		// call sense()
	struct sockaddr_in servAddr = createServer(port);  // create Server
	struct sockaddr_in dataAddr;

	if (bind(listenfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < NORMAL) {  // check for bind error
		perror("bind error");
		exit(1);
	}

	listen(listenfd, 4);									// listen() 
	socklen_t length = sizeof(struct sockaddr_in);			// grab socket length
	struct sockaddr_in clientAddr;			
	struct hostent* hostEntry;									
	char* hostName;



	while (TRUE) { 
		int connectfd = accept(listenfd, (struct sockaddr *) &clientAddr, &length);
		if (connectfd < NORMAL){
			fprintf(stderr, "Connection could not be made!\n");
			return FALSE;
		}
		int check = fork();
		if (!(check)){				
			hostEntry = gethostbyaddr(&(clientAddr.sin_addr), sizeof(struct in_addr), AF_INET);
			hostName = hostEntry->h_name;
			char* ttime =grabTime();
			printf("Child: %d ", getpid());
			printf("Connection was made from %s at %s\n", hostName, ttime);
			while(TRUE){


				if (!read(connectfd, &buffferChar, TRUE)){
					printf("Error!\nThe Connection was dropped.\n");
					return 0;
				}

				readBuffer(buffferChar, buffer, connectfd);

				// server responses 
				if(buffer[0] == 'Q'){
						printf("Client from %s disconnected.\n",  hostName);

						write(connectfd, "A\n", replyLength);
						exit(0);
				}



				if(buffer[0] == 'G'){
						if(listendatafd == FALSE){
							printf("Error\n Data connection could not be established.\n");
						} else {
							get(datafd, connectfd, buffer+1);
							close(datafd);
							close(listendatafd);
							datafd = 0;
							listendatafd = datafd;
						}
				}

				//data
				if(buffer[0] == 'D'){
						listendatafd = sense(port);
						dataAddr = createServer(0);
						portGrab = grabPort(listendatafd, dataAddr);
						strcpy(reply, "A");
						strcat(reply, portGrab);
						strcat(reply, "\n");
						write(connectfd, reply, (int) strlen(reply));	
						listen(listendatafd, 1);
						datafd = dataConnection(listendatafd);
	
				}


				//reply
				if(buffer[0] == 'L'){
						if(!listendatafd){
							printf("Error!\n Data connection could not be established.\n");
						} else {
							rls(datafd);
							write(connectfd, "A\n", replyLength);
							close(datafd);
							close(listendatafd);
							datafd = 0;
							listendatafd = datafd;
						}
				}



				//rcd message
				if(buffer[0] == 'C'){
						printf("%s", buffer);
						if ((rcd(buffer+1) != FALSE) ){
						write(connectfd, "A\n", replyLength);
						} else{
						//char* type = "ERCD failed\n";
						//int lengthType = sizeof(type);
						write(connectfd, "ERCD failed.\n", writeLength);
					}
				}


				//data connection
				if(buffer[0] == 'P'){
					if(!listendatafd){
							printf("Error\n Data connection could not be established.\n");
						} else {
							put(datafd, connectfd, buffer+1);
							close(datafd);
							close(listendatafd);
							datafd = 0;
							listendatafd = datafd;
						}
					}	
				}
			}
			close(connectfd);
		}
	free(portGrab);
    return NORMAL;
}
	//#pragma GCC diagnostic pop 
