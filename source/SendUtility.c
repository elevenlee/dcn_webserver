////////////////////////////////////////////////////////////
/*
	File Name:		SendUtility.c
	Instructor:		Prof. Arthur Goldberg
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This SendUtility.c file includes 
					Send Server Response Message Functions.
*/
////////////////////////////////////////////////////////////

///////////////HEADER FILES///////////////
#include "Server.h"

// NC: THIS looks like good code, but our web server didn't read or send files.

///////////////FUNCTIONS///////////////
/*List File Name in Directory Function
  Variable Definition:
  -- url: the request url except domain name and port number
  -- method: the request method
  -- client_socket: socket connected to the client
  Return Value: NULL
*/
void sendFileNameInDirectory(char *url, const char *method, int client_socket){
	char 			*entity_body = (char*)malloc(sizeof(char) * (BUFFER_SIZE + 1));	//files in directory string
	int				dir_length = 0;													//directory information length
	struct dirent 	*directory;														//save the file information in directory
	DIR 			*dir;															//representing directory stream
	
	sprintf(entity_body, "The List of Directory /%s\n\n", url);
	//Open the directory
	if ((dir = opendir(url)) != NULL){
		//read the files information in directory
		while ((directory = readdir(dir))){
			//Output the files name
			strcat(entity_body, directory->d_name);
			strcat(entity_body, "\n");
		}
		//Close the directory stream
		closedir(dir);
	}
	//Set the value of directory information length
	dir_length = strlen(entity_body) + 1;
	//HEAD request does not Response entity body
	if (strcmp(method, "HEAD") == 0){
		entity_body = NULL;
	}
	//Send Response message to the client
	constructResponseMessage(	client_socket,
								NULL,
								200,
								"OK",
								NULL,
								dir_length,
								"text/plain",
								entity_body);
}

// NC: THIS looks like good code, but we don't need extensions.

/*Send OK Function
  Variable Definition:
  -- url: the request url except domain name and port number
  -- method: the request method
  -- client_socket: socket connected to the client
  Return Value: NULL
*/
void sendOK(char *url, const char *method, int client_socket){
	char 	*extension = getFileType(url);										//file extension
	char 	*type = "text/html";												//file type
	char	*entity_body = (char*)malloc(sizeof(char) * (FILE_SIZE + 1));		//entity body
	FILE 	*out_channel;														//file stream to client socket
	int 	file_length = 0;													//file length
	int 	bytes = 0;															//bytes that has sent
	
	//Set the value of Content-Type
	if (strcmp(extension, "txt") == 0){
		type = "text/plain";
	}
	else if (strcmp(extension, "gif") == 0){
		type = "image/gif";
	}
	else if (strcmp(extension, "jpg") == 0){
		type = "image/jpeg";
	}
	else if (strcmp(extension, "jpeg") == 0){
		type = "image/jpeg";
	}
	
	//Create an output stream to the socket
	out_channel = fdopen(client_socket, "w");
	if (out_channel != NULL){
		//Get the request file content and file length
		file_length = getFileContent(url, entity_body);
		//HEAD request does not Response entity body
		if (strcmp(method, "HEAD") == 0){
			entity_body = NULL;
		}
		//Send Response Message to client
		bytes = constructResponseMessage(	client_socket,
											&out_channel,
											200,
											"OK",
											convertTimeFormat(getTimeInGMTFormat(url, 2), 1),
											file_length,
											type,
											entity_body);
		//Close file stream
		fclose(out_channel);
	}
}

/*Send Not Modified Function
  Variable Definition:
  -- url: the request url except domain name and port number
  -- client_socket: socket connected to the client
  Return Value: NULL
*/
void sendNotModified(char *url, int client_socket){
	//Send Response Message to client
	constructResponseMessage(	client_socket,
								NULL,
								304,
								"Not Modified",
								convertTimeFormat(getTimeInGMTFormat(url, 2), 1),
								0,
								NULL,
								NULL);
}

/*Send Bad Request Function
  Variable Definition:
  -- detail: information about bad request
  -- client_socket: socket connected to the client
  Return Value: NULL
*/
void sendBadRequest(const char *detail, int client_socket){	
	char *entity_body = (char*)malloc(sizeof(char) * (HALFBUF_SIZE + 1));	//entity body
	
	//Set the entity body
	sprintf(entity_body, "400 Bad Request -- The request could not be understood by the server! (%s)", detail);
	//Send Response Message to client
	constructResponseMessage(	client_socket,
								NULL,
								400,
								"Bad Request",
								NULL,
								strlen(entity_body) + 1,
								"text/html",
								entity_body);
}

/*Send Not Found Function
  Variable Definition:
  -- url: the request url except domain name and port number
  -- client_socket: socket connected to the client
  Return Value: NULL
*/
void sendNotFound(char *url, int client_socket){
	char *entity_body = (char*)malloc(sizeof(char) * (HALFBUF_SIZE + 1));	//entity body
	
	//Set the entity body
	sprintf(entity_body, "404 Not Found -- The requested URL /%s does not exist on this server!", url);
	//Send Response Message to client
	constructResponseMessage(	client_socket,
								NULL,
								404,
								"Not Found",
								NULL,
								strlen(entity_body) + 1,
								"text/html",
								entity_body);
}

/*Send Method Not Allowed Function
  Variable Definition:
  -- method: the request method
  -- client_socket: socket connected to the client
  Return Value: NULL
*/
void sendMethodNotAllowed(const char *method, int client_socket){
	char *entity_body = (char*)malloc(sizeof(char) * (HALFBUF_SIZE + 1));	//entity body
	
	//Set the entity body
	sprintf(entity_body, "405 Method Not Allowed -- %s is not allowed!", method);
	//Send Response Message to client
	constructResponseMessage(	client_socket,
								NULL,
								405,
								"Method Not Allowed",
								NULL,
								strlen(entity_body) + 1,
								"text/html",
								entity_body);
}

/*Send HTTP Version Not Supported
  Variable Definition:
  -- version: the request HTTP version
  -- client_socket: socket connected to the client
  Return Value: NULL
*/
void sendHTTPVersionNotSupported(const char *version, int client_socket){
	char *entity_body = (char*)malloc(sizeof(char) * (HALFBUF_SIZE + 1));	//entity body
	
	//Set the entity body
	sprintf(entity_body, "505 HTTP Version Not Supported -- %s is not supported by the server!", version);
	//Send Response Message to client
	constructResponseMessage(	client_socket,
								NULL,
								505,
								"HTTP Version Not Supported",
								NULL,
								strlen(entity_body) + 1,
								"text/html",
								entity_body);
}
