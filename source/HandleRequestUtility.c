/////////////////////////////////////////////////////////////////
/*
	File Name:		HandleRequestUtility.c
	Instructor:		Prof. Arthur Goldberg
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This HandleRequestUtility.c file includes 
					Handle Client Request Functions.
*/
/////////////////////////////////////////////////////////////////

///////////////HEADER FILES///////////////
#include "Server.h"

///////////////FUNCTIONS///////////////
/*Handle Client Request Function
  Variable Definition:
  -- client_socket: socket connected to the client
  Return Value: NULL
*/
void handleClientRequest(int client_socket){
	char 	request_line[STRING_SIZE];		//client request line
	char	*return_value;					//each line header pointer
	FILE 	*channel;						//file stream for client socket
	
	//Initialize request_line buffer
	memset(request_line, 0, STRING_SIZE);
	//Create an input stream from the socket
	channel = fdopen(client_socket, "r");
	if (channel == NULL){
		dieWithSystemMessage("fdopen() failed!");
	}
	
	//Get client Request Line (Jump the blank line)
	// NC: i don't understand why there's a blank line
	do{
		return_value = fgets(request_line, STRING_SIZE, channel);
	}while (syntaxChecking(return_value, 'B'));
	
	// NC: what if the request line doesn't fit in request_line[STRING_SIZE]?
	
	//Output the client_socket id and Request Line
	printf("Got a call on %d: request = %s", client_socket, request_line);
	//Get client Header Lines & Response the client request
	respondClientRequest(request_line, getHeaderLines(channel), client_socket);
	
	//Close client socket
	close(client_socket);
	//Close file stream
	fclose(channel);
}

// NC: VERY, VERY well organized

/*Respond Client Request Function
  Variable Definition:
  -- request: client request line
  -- header: client header lines
  -- client_socket: socket connected to the client
  Return Value: NULL
*/
void respondClientRequest(char *request, struct http_header *header, int client_socket){
	char 	method[STRING_SIZE];		//method field: GET or HEAD
	char 	url[STRING_SIZE];			//url field: /TestServer
	char 	version[STRING_SIZE];		//http version field: HTTP/1.1
	char	field_value[HALFBUF_SIZE];	//field value string
	bool	modified_signal;			//whether if-modified-since field exist
	
	//Initialize method, url and version buffer
	memset(method, 0, STRING_SIZE);
	memset(url, 0, STRING_SIZE);
	memset(version, 0, STRING_SIZE);
	memset(field_value, 0, HALFBUF_SIZE);
	
	//Test the client HTTP Request Line
	if (!syntaxChecking(request, 'R')){
		//400 Bad Request: the request could not be understood by the server
		sendBadRequest("Request Line is syntactically incorrect!", client_socket);
		return;
	}
	
	//Get the method, url, and http version
	// NC: i don't think this works: need   sscanf(request, "%s %s %s", method, url, version);
	sscanf(request, "%s%s%s", method, url, version);
	//Decode the URL(if it has %HEX code)
	decodeURL(url);
	//Test the method
	if ((strcmp(method, "GET") != 0) && (strcmp(method, "HEAD") != 0)){
		//405 Method Not Allowed: the method field is neither "GET" nor "HEAD"
		sendMethodNotAllowed(method, client_socket);
		return;
	}
	//Test the Requested URL
	else if (!syntaxChecking(url, 'U')){
		// NC: but, "the only legal Request-URI is the abs_path “/TestServer”. Generate a 404 Response to all other Request-URIs."
		// #17: -1
		//400 Bad Request: the request could not be understood by the server
		sendBadRequest("Requested URL is syntactically incorrect!", client_socket);
		return;
	}
	//Test the HTTP version
	else if (!syntaxChecking(version, 'V')){
		//400 Bad Request: the request could not be understood by the server
		sendBadRequest("HTTP Version is syntactically incorrect!", client_socket);
		return;
	}
	else if (!syntaxChecking(version, 'O')){
		//505 HTTP Version Not Supported: the requested HTTP protocol version is not supported by server
		sendHTTPVersionNotSupported(version, client_socket);
		return;
	}
	
	//Test the Header Line
	if (headerLinesIncorrect(header, field_value)){
		//400 Bad Request: the request could not be understood by the server
		sendBadRequest(field_value, client_socket);
		return;
	}
	//Test the "Host" field
	else if (fieldNotExist(header, "host", field_value)){
		//400 Bad Request: the request could not be understood by the server
		sendBadRequest("Host field does not exist!", client_socket);
		return;
	}
	else if (!syntaxChecking(field_value, 'I')){
		//400 Bad Request: the request could not be understood by the server
		sendBadRequest("Host field value(domain name, IP address, or port format) is syntactically incorrect!", client_socket);
		return;
	}
	//Test the "If-Modified-Since" field
	else if ((!(modified_signal = fieldNotExist(header, "if-modified-since", field_value))) && (!syntaxChecking(field_value, 'T'))){
		//400 Bad Request: the request could not be understood by the server
		sendBadRequest("If-Modified-Since field value(time format) is syntactically incorrect!", client_socket);
		return;
	}
	
	//Now we are sure that the request message is SYNTACTICALLY CORRECT
	//Get the Requested File or Directory's name
	pathBelowCurrentDirectory(url);
	printf("Requested File or Directory is %s\n", url);
// NC: but OUR server doesn't do this
	
	//Test the requested file on the server
	if (urlNotExist(url)){
		//404 Not Found: the requested document does not exist on this server
		sendNotFound(url, client_socket);
	}
	else if (urlIsADirectory(url)){
		//List the File Name of Directory
		sendFileNameInDirectory(url, method, client_socket);
	}
	else if (modified_signal){
		//200 OK: the request is good
		sendOK(url, method, client_socket);
	}
	else if (compareModifiedTime(url, field_value)){
		//304 Not Modified: the request does not Modified since If-Modified-Since field
		sendNotModified(url, client_socket);
	}
	else{
		//200 OK: the request is good
		sendOK(url, method, client_socket);
	}

	return;
}

/*Construct Response Message to Client Function
  Variable Definition:
  -- client_socket: socket connected to the client
  -- stream: file stream of socket or server file
  -- status_code: response status code
  -- status_message: response status message
  -- last_modified_time: response last modified time field
  -- content_length: response content length field
  -- content_type: response content type field
  -- content: response entity body field
  Return Value: bytes of whole message
*/
int	constructResponseMessage(	int			client_socket,
								FILE 		**stream,
								int			status_code,
								const char	*status_message,
								const char	*last_modified_time,
								int			content_length,
								const char	*content_type,
								const char	*content){
	int		bytes = 0;		//bytes that send to client
	FILE	*channel;		//file stream for client socket
	
	//Create an output stream to the socket
	channel = fdopen(client_socket, "w");
	if (channel != NULL){
		//Response Message Status Line
		bytes = fprintf(channel, "HTTP/1.1 %d %s%s", status_code, status_message, CRLF);
		//Response Date field
		bytes += fprintf(channel, "Date: %s%s", convertTimeFormat(getTimeInGMTFormat(NULL, 0), 1), CRLF);
		//Response Server field
		bytes += fprintf(channel, "Server: Data Communications & Networks: HTTP server by %s (Unix)%s", MY_NAME, CRLF);
		//Response Last-Modified field
		if (last_modified_time){
			bytes += fprintf(channel, "Last-Modified: %s%s", last_modified_time, CRLF);
		}
		//Response Content-Length field
		if (content_length != 0){
			bytes += fprintf(channel, "Content-Length: %d%s", content_length, CRLF);
		}
		//Response Message Content-Type field
		if (content_type){
			bytes += fprintf(channel, "Content-Type: %s%s%s", content_type, CRLF, CRLF);
		}
		//Response Message Entity Body field
		if (content){
			bytes += fprintf(channel, "%s%s", content, CRLF);
		}
	}
	//Flush the channel
	fflush(channel);
	
	//Pass the stream
	if (stream){
		*stream = channel;
	}
	else{
		//Close file stream
		fclose(channel);
	}
	
	return bytes;
}

/*Get Header Lines Function
  Variable Definition:
  -- stream: file stream for client socket
  Return Value: header pointer of header lines structure
*/
struct http_header *getHeaderLines(FILE *stream){
	char 				buffer[HALFBUF_SIZE];												//header line string
	char 				*field_name;														//header field name
	int 				number = 0;															//number of header lines
	struct http_header 	*header = (struct http_header*)malloc(sizeof(struct http_header));	//http_header structure head pointer
	struct http_header 	*node;																//http_header structure node
	
	//Initialize buffer
	memset(buffer, 0, HALFBUF_SIZE);
	//Initialize the structure http_header head node
	strcpy(header->header_line, "");
	strcpy(header->field_name, "HTTP Request Header Lines");
	strcpy(header->field_value, "0");
	header->next = NULL;
	
	//Let the node pointer point to the header
	node = header;
	//Get the header lines
	while (fgets(buffer, HALFBUF_SIZE, stream) != NULL && strcmp(buffer, CRLF) != 0){
		//Allocate the memory for new node
		node->next = (struct http_header*)malloc(sizeof(struct http_header));
		//Let the node pointer point to the current header line
		node = node->next;
		//Assign the variable(header_line)
		strcpy(node->header_line, buffer);
		//Split the Field Name and Field Value (According to the ':')
		field_name = splitNameAndValue(buffer, ':');
		//Assign the variable(field_name and field_value)
		strcpy(node->field_name, field_name);
		strcpy(node->field_value, buffer);
		node->next = NULL;
		//Count the number of header lines
		number++;
	}
	//Set the value of header pointer's field_value (the number of header lines)
	strcpy(header->field_value, itoa(number));
	
	return header;
}
