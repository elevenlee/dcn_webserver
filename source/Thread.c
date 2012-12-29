///////////////////////////////////////////////////////
/*
	File Name:		Thread.c
	Instructor:		Prof. Arthur Goldberg
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This Thread.c file
					includes Thread Main Function.
*/
///////////////////////////////////////////////////////

///////////////HEADER FILES///////////////
#include "Server.h"

///////////////FUNCTIONS///////////////
/*Thread Main Function
  Variable Definition:
  -- thread_arguments: arguments which thread should be used
  Return Value: NULL
*/
void *threadMain(void *thread_arguments){
	int client_socket;		//socket descriptor for client
	
	//Guarantees that thread resources are deallocated upon return
	pthread_detach(pthread_self());
	//Pass the arguments
	client_socket = ((struct thread_arguments*)thread_arguments)->client_socket;
	//Deallocate memory for argument
	free(thread_arguments);
	//Handle the client request
	handleClientRequest(client_socket);
	
	return (NULL);
}
