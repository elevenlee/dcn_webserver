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
#include "Client.h"

///////////////FUNCTIONS///////////////
/*Main Function
  Variable Definition:
  -- argc: the number of command arguments
  -- argv[]: each vairable of command arguments(argv[0] is the path of execution file forever)
  Return Value: client exit number
*/
int main(int argc, char *argv[]){
	char					*host = IP_ADDRESS;			//host name/ip address
	char					*service = PORT_NUMBER;		//service/port number
	int						client_socket = -1;			//socket descriptor for client
	FILE					*in_channel;				//file stream for read file
	FILE					*out_channel;				//file stream for write file
	struct request_message	*node;						//request_message structure node

	//Open file for read request message
	if ((in_channel = fopen(READ_FILE_NAME, "r")) == NULL){
		dieWithUserMessage("fopen() failed!", "read file cannot open!");
	}
	//Open file for write response message
	if ((out_channel = fopen(WRITE_FILE_NAME, "w")) == NULL){
		dieWithUserMessage("fopen() failed!", "write file cannot open!");
	}
	//Read request message file and get the request messages
	for (node = (getRequestMessage(in_channel))->next; node != NULL; node = node->next){
		//Create a connected TCP socket
		client_socket = setupClientSocket(host, service);
		if (client_socket < 0){
			dieWithUserMessage("setupClientSocket() failed!", "Unable to connect");
		}
		//Send and receive message
		fputs(sendAndRcvdMessage(client_socket, node), out_channel);
		fputs(CRLF, out_channel);
		fputs(CRLF, out_channel); // NC; you don't need multiple blank lines, but it doesnt matter since the TCP connection is being closed
		fputs(CRLF, out_channel);
		//Close the socket
		close(client_socket);
	}
	//Close read file
	fclose(in_channel);
	//Close write file
	fclose(out_channel);
	
	return 0;
}
