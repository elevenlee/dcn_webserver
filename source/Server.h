////////////////////////////////////////////////////////////////////////////////////
/*
	File Name:		Server.h
	Instructor:		Prof. Arthur Goldberg
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This Server.h file includes
					HEADER FILES, MACRO, STRUCT DEFINITION,
					GLOBAL VARIABLE AND FUNCTION DECLARATION.
*/
////////////////////////////////////////////////////////////////////////////////////

///////////////PRECOMPILER///////////////
#ifndef	SERVER_H_
#define SERVER_H_

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
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <dirent.h>
#include <time.h>
#include <regex.h>

///////////////MACRO///////////////
#define	MAX_PENDING		10
#define LOCAL_ADDRESS	"127.0.0.1"
#define	PORT_NUMBER		"5678"
#define	MY_NAME			"Shen Li"
#define	CRLF			"\r\n"

///////////////GLOBAL VARIABLE///////////////
enum size_constants{
	ONE_SIZE		= 1,
	NUMBER_SIZE 	= 4,
	REPLACE_SIZE	= 6,
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
struct thread_arguments		//structure of arguments to pass to client thread
{
	int client_socket;		//socket descriptor for client
};

struct http_header			//structure of http request message
{
	char header_line[HALFBUF_SIZE];	//header line
	char field_name[HALFSTR_SIZE];	//field name
	char field_value[HALFBUF_SIZE];	//field value
	struct http_header *next;		//next pointer
};

///////////////FUNCTION DECLARATION///////////////
/*DieWithMessage.c*/
void 	dieWithUserMessage(const char *message, const char *detail);
void 	dieWithSystemMessage(const char *message);
/*ServerUtility.c*/
int 	setupServerSocket(const char *service);
int 	acceptConnection(int server_socket);
/*AddressUtility.c*/
void 	printSocketAddress(const struct sockaddr *address, FILE *stream);
char	*getLocalIPAddress();
char	*getLocalDomainName();
/*Thread.c*/
void	*threadMain(void *thread_arguments);
/*HandleRequestUtility.c*/
void 	handleClientRequest(int client_socket);
void 	respondClientRequest(char *request, struct http_header *header, int client_socket);
int		constructResponseMessage(	int			client_socket,
									FILE		**stream,
									int			status_code,
									const char	*status_message,
									const char	*last_modified_time,
									int			content_length,
									const char	*content_type,
									const char	*content);
struct http_header 	*getHeaderLines(FILE *stream);
/*SendUtility.c*/
void	sendFileNameInDirectory(char *url, const char *method, int client_socket);
void	sendOK(char *url, const char *method, int client_socket);
void	sendNotModified(char *url, int client_socket);
void 	sendBadRequest(const char *detail, int client_socket);
void	sendNotFound(char *url, int client_socket);
void	sendMethodNotAllowed(const char *method, int client_socket);
void	sendHTTPVersionNotSupported(const char *version, int client_socket);
/*FileUtility.c*/
char	*getFileType(char *url);
int		getFileContent(char *url, char *content);
/*TimeUtility.c*/
struct tm	*getTimeInGMTFormat(char *url, int signal_value);
char	*convertTimeFormat(struct tm *gmt_time, int signal_value);
bool	compareModifiedTime(char *url, char *modified_time_string);
/*StringUtility.c*/
char 	*itoa(int number);
void	decodeURL(char *url);
void	pathBelowCurrentDirectory(char *url);
bool	headerLinesIncorrect(struct http_header *header, char *field_value);
bool	urlNotExist(char *url);
bool	fieldNotExist(struct http_header *header, const char *field_name, char *field_value);
bool	urlIsADirectory(char *url);
bool	syntaxChecking(char *string, const char signal_value);
char	*splitNameAndValue(char *header_line, const char stop);
int		replaceString(char *content, int length, const char *original, const char *replace);

#endif //SERVER_H
