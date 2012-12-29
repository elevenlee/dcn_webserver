////////////////////////////////////////////////////////////////////////////////////
/*
	File Name:		Client.h
	Instructor:		Prof. Arthur Goldberg
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This Client.h file includes
					HEADER FILES, MACRO, STRUCT DEFINITION,
					GLOBAL VARIABLE AND FUNCTION DECLARATION.
*/
////////////////////////////////////////////////////////////////////////////////////

///////////////PRECOMPILER///////////////
#ifndef	CLIENT_H_
#define CLIENT_H_

///////////////DEBUG///////////////
//#define DEBUG 1
#ifdef DEBUG
	#define DEBUG_PRINT printf("%s-%s:%d:", __FILE__, __FUNCTION__, __LINE__)
#else
	#define DEBUG_PRINT
#endif

///////////////HEADER FILES///////////////
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

///////////////MACRO///////////////
#define IP_ADDRESS		"localhost"
#define PORT_NUMBER		"5678"
#define READ_FILE_NAME	"Request_Message.txt"
#define WRITE_FILE_NAME	"Response_Message.txt"
#define CRLF			"\r\n"

///////////////GLOBAL VARIABLE///////////////
enum size_constants{
	ONE_SIZE		= 1,
	NUMBER_SIZE 	= 4,
	DATE_SIZE 		= 8,
	NAME_SIZE 		= 16,
	TIME_SIZE 		= 32,
	HALFSTR_SIZE 	= 64,
	STRING_SIZE 	= 128,
	HALFBUF_SIZE 	= 512,
	BUFFER_SIZE 	= 1024,
	DOUBUF_SIZE		= 2048,
	FILE_SIZE 		= 65536,
};

///////////////STRUCT DEFINITION///////////////
struct request_message{				//structure of http request message
	char message[BUFFER_SIZE];		//entity message
	size_t message_length;			//message length
	struct request_message *next;	//next pointer
};

///////////////FUNCTION DECLARATION///////////////
/*DieWithMessage.c*/
void	dieWithUserMessage(const char *message, const char *detail);
void	dieWithSystemMessage(const char *message);
/*ClientUtility.c*/
int		setupClientSocket(const char *host, const char *service);
struct request_message *getRequestMessage(FILE *stream);
char	*sendAndRcvdMessage(int client_socket, struct request_message *node);

#endif //CLIENT_H
