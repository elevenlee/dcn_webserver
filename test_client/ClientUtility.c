////////////////////////////////////////////////////////
/*
	File Name:		ClientUtility.c
	Instructor:		Prof. Arthur Goldberg
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This ClientUtility.c file includes 
					Setup Socket, Get Request Message,
					and SendRcvd MessageFunction.
*/
////////////////////////////////////////////////////////

///////////////HEADER FILES///////////////
#include "Client.h"

///////////////FUNCTIONS///////////////
/*Setup Client Socket Function
  Variable Definition:
  -- host: socket host name or ip address
  -- service: socket service name or port number
  Return value: socket number
*/
int setupClientSocket(const char *host, const char *service){
	struct addrinfo		address_criteria;		//criteria for address match
	struct addrinfo		*server_address;		//holder for returned list of server addresses
	struct addrinfo		*address;				//pointer to addresses node
	int					client_socket = -1;		//socket descriptor for client
	int					return_value;			//return value
	
	//Tell the system what kind(s) of address info we want
	memset(&address_criteria, 0, sizeof(address_criteria));	//zero the address_criteria
	address_criteria.ai_family = AF_UNSPEC;					//any address family
	address_criteria.ai_socktype = SOCK_STREAM;				//only streaming sockets
	address_criteria.ai_protocol = IPPROTO_TCP;				//only TCP protocol
	
	//Get address(es)
	return_value = getaddrinfo(host, service, &address_criteria, &server_address);
	//Success returns zero
	if (return_value != 0){
		dieWithUserMessage("getaddrinfo() failed!", gai_strerror(return_value));
	}
	
	//Create socket for connecting the server
	for (address = server_address; address != NULL; address = address->ai_next){
		//Create a reliable, stream socket using TCP
		client_socket = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
		//Socket creation failed; try next address
		if (client_socket < 0){
			continue;
		}
		//Establish the connection to the server
		if (connect(client_socket, address->ai_addr, address->ai_addrlen) == 0){
			//Socket connection succeeded; break and return socket
			break;
		}
		//Socket connection failed; try next address
		close(client_socket);
		client_socket = -1;
	}
	//Free addrinfo allocated in getaddrinfo()
	freeaddrinfo(server_address);
	
	return client_socket;
}

/*Get Request Message from local file Function
  Variable Definition:
  -- stream: file stream for local file
  Return Value: header pointer of request message structure
*/
struct request_message *getRequestMessage(FILE *stream){
	char					line_string[STRING_SIZE];													//each line string
	char					message_string[BUFFER_SIZE];												//each message string
	int						number = 0;																	//number of request message
	struct request_message	*header = (struct request_message*)malloc(sizeof(struct request_message));	//request_message structure header pointer
	struct request_message	*node;																		//request_message structure node

	//Initialize buffer
	memset(line_string, 0, STRING_SIZE);
	memset(message_string, 0, BUFFER_SIZE);
	//Initialize the structure http_header head node
	sprintf(header->message, "Number of Request Message%s", CRLF);
	header->message_length = 0;
	header->next = NULL;
	
	//Let the node pointer point to the header
	node = header;
	//Get the request message from file stream
	while (fgets(line_string, STRING_SIZE, stream) != NULL){
		//Delete the last character '\n'
		line_string[strlen(line_string) - 1] = '\0';
		//Add the character CRLF to each line
		strcat(line_string, CRLF);
		//Add each line string to entity message string
		strcat(message_string, line_string);
		//Test the line only with CRLF
		if (strcmp(line_string, CRLF) == 0){
			//Allocate the memory for new node
			node->next = (struct request_message*)malloc(sizeof(struct request_message));
			//Let the node pointer point to the current header line
			node = node->next;
			//Assign the variable(message and message_length)
			strcpy(node->message, message_string);
			node->message_length = strlen(message_string);
			node->next = NULL;
			//Count the number of request messages
			number++;
			//Zero the message_string buffer
			memset(message_string, 0, BUFFER_SIZE);
		}
	}
	//Set the value of header pointer's message_length (the number of request messages)
	header->message_length = number;

	return header;
}

/*Send Request Message and Receive Response Message Function
  Variable Definition:
  -- client_socket: socket connected to the server
  -- node: request message node
  Return Value: received response message from server
*/
char *sendAndRcvdMessage(int client_socket, struct request_message *node){	
	char		*rcvd_buffer = (char*)malloc(sizeof(char) * (BUFFER_SIZE));	//received response message buffer
	ssize_t		bytes = 0;													//number of bytes

	//send request message to the server
	bytes = send(client_socket, node->message, node->message_length, MSG_NOSIGNAL);
	//Test the send is successful
	if (bytes < 0){
		dieWithSystemMessage("send() failed!");
	}
	else if (bytes != node->message_length){
		dieWithUserMessage("send()", "sent unexpected number of bytes");
	}
	//Receive Response Message from the server
	bytes = recv(client_socket, rcvd_buffer, BUFFER_SIZE - 1, 0);
	//Test the receive is successful
	if (bytes < 0){
		dieWithSystemMessage("recv() failed!");
	}
	else if (bytes == 0){
		dieWithUserMessage("recv()", "connection closed prematurely");
	}
	// NC: cool automated testing; you could also check the content of the result
	return rcvd_buffer;
}
