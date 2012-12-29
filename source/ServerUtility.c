///////////////////////////////////////////////////////////////////////////
/*
	File Name:		ServerUtility.c
	Instructor:		Prof. Arthur Goldberg
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This ServerUtility.c file includes 
					Setup Socket Function and Connect Socket Function.
*/
///////////////////////////////////////////////////////////////////////////

///////////////HEADER FILES///////////////
#include "Server.h"

///////////////FUNCTIONS///////////////
/*Setup Server Socket Function
  Variable Definition:
  -- service: socket service name or port number
  Return value: server socket number
*/
int setupServerSocket(const char *service){
	struct addrinfo 		address_criteria;	//criteria for address match
	struct addrinfo 		*server_address;	//list of server addresses
	struct addrinfo 		*address;			//pointer to addresses node
	struct sockaddr_storage local_address;		//print local address
	socklen_t 				address_size;		//address size
	int 					server_socket = -1;	//socket descriptor for server
	int 					return_value;		//return value
	
	//Construct the server address structure
	memset(&address_criteria, 0, sizeof(address_criteria));	//zero the address_criteria
	address_criteria.ai_family = AF_UNSPEC;					//any address family
	address_criteria.ai_flags = AI_PASSIVE;					//accept on any address/port
	address_criteria.ai_socktype = SOCK_STREAM;				//only stream sockets
	address_criteria.ai_protocol = IPPROTO_TCP;				//only tcp protocol
	
	//Get address/name information
	return_value = getaddrinfo(NULL, service, &address_criteria, &server_address);
	//Success returns zero
	if (return_value != 0){
		dieWithUserMessage("getaddrinfo() failed!", gai_strerror(return_value));
	}
	
	//Create socket for incoming connections
	for (address = server_address; address != NULL; address = address->ai_next){
		//Initialize the server socket
		server_socket = -1;
		//Create socket for incoming connections
		server_socket = socket(server_address->ai_family, server_address->ai_socktype, server_address->ai_protocol);
		//if socket creation failed, try next address in the list
		if (server_socket < 0){
			continue;
		}
		
		//Bind to the server local address and set socket to the list
		if ((bind(server_socket, server_address->ai_addr, server_address->ai_addrlen) == 0) && 
				(listen(server_socket, MAX_PENDING) == 0)){
			//Get address size
			address_size = sizeof(local_address);
			//Get socket name
			if (getsockname(server_socket, (struct sockaddr*)&local_address, &address_size) < 0){
				dieWithSystemMessage("getsockname() failed!");
			}
			//Output local address and port of socket(listening address and port)
			fputs("Binding to ", stdout);
			printSocketAddress((struct sockaddr*)&local_address, stdout);
			fputc('\n', stdout);
			//Bind and list successful
			break;
		}
		//Close and try again
		close(server_socket);
	}
	//Free address list allocated by getaddrinfo()
	freeaddrinfo(server_address);
	
	return server_socket;
}

/*Accept Client Connection Function
  Variable Definition:
  -- server_socket: server socket number
  Return Value: connected client socket number
*/
int acceptConnection(int server_socket){
	struct sockaddr_storage client_address;			//client address
	socklen_t 				client_address_length;	//length of client address structure
	int 					client_socket;			//socket descriptor for client
	
	//Set length of client address structure
	client_address_length = sizeof(client_address);
	//Wait for a client to connect
	client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_length);
	if (client_socket < 0){
		dieWithSystemMessage("accept() failed!");
	}
	
	//Now, client_socket is connected to a client
	//Output the socket address and port
	fputs("Handling client ", stdout);
	printSocketAddress((struct sockaddr*)&client_address, stdout);
	fputc('\n', stdout);
	
	return client_socket;
}
