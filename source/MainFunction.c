///////////////////////////////////////////////
/*
	File Name:		MainFunction.c
	Instructor:		Prof. Arthur Goldberg
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This MainFunction.c file
					includes Main Function.
*/
///////////////////////////////////////////////

///////////////HEADER FILES///////////////
#include "Server.h"

///////////////FUNCTIONS///////////////
/*Main Function
  Variable Definition:
  -- argc: the number of command arguments
  -- argv[]: each vairable of command arguments(argv[0] is the path of execution file forever)
  Return Value: Server exit number
*/
int main(int argc, char *argv[]){
	char	*service = PORT_NUMBER;	//server listening port number
	int 	server_socket;			//socket descriptor for server
	int 	client_socket;			//socket descriptor for client
	int		return_value;			//return value
	
	/*
	 Pass the port number to your server in a command line argument.
	 update' is a command line parameter.
	 #3: -2
	 */
	//Create socket for incoming connections
	server_socket = setupServerSocket(service);
	if (server_socket < 0){
		dieWithSystemMessage("setupServerSocket() failed!");
	}
	
	//Run forever
	for (;;){
		//New connection creates a connected client socket
		client_socket = acceptConnection(server_socket);
		
		//Create seperate memory for client argument
		struct thread_arguments *thread_args = (struct thread_arguments*)malloc(sizeof(struct thread_arguments));
		if (thread_args == NULL){
			dieWithSystemMessage("malloc() failed!");
		}
		//Pass the arguments to client socket
		thread_args->client_socket = client_socket;
		
		//Create client thread
		pthread_t thread_id;			//thread id number
		return_value = pthread_create(&thread_id, NULL, threadMain, thread_args);
		if (return_value != 0){
			dieWithUserMessage("pthread_create() failed!", strerror(return_value));
		}
		//Output the thread id
		printf("with thread %lu\n", (unsigned long int)thread_id);
	}
	
	return 0;
}
