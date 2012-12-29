////////////////////////////////////////////////////////////
/*
	File Name:		FileUtility.c
	Instructor:		Prof. Arthur Goldberg
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This FileUtility.c file includes 
					Handle Server File Functions.
*/
////////////////////////////////////////////////////////////

///////////////HEADER FILES///////////////
#include "Server.h"

///////////////FUNCTIONS///////////////
/*Get File Type Function
  Variable Definition:
  -- url: the request url except domain name and port number
  Return Value: pointer to the file extension type
*/
char *getFileType(char *url){
	char *extension = NULL;		//file extension
	
	//Get the file extension type by '.' character
	if ((extension = strrchr(url, '.')) != NULL){
		//Jump the '.' character
		return (extension + 1);
	}
	return "";
}

// NC: nice and general purpose, 

/*Get File Content Function
  Variable Definition:
  -- url: the request url except domain name and port number
  -- content: file content string
  Return Value: file length
*/
int getFileContent(char *url, char *content){
	int		i = 0;					//counter
	int 	file_length;			//file length
	FILE	*in_channel;			//file stream
	
	//Create an input stream from the local file
	if ((in_channel = fopen(url, "r")) == NULL){
		dieWithSystemMessage("fopen() failed!");
	}
	//Get the file content
	content[i++] = fgetc(in_channel);
	while (!feof(in_channel)){
		content[i++] = fgetc(in_channel);
	}
	//Close the file
	fclose(in_channel);
	//Set the value of file length
	file_length = i - 1;
	
	//Replace the TestServer file
	if (strcmp(url, "TestServer") == 0){
		char *original_string[REPLACE_SIZE] = {	"<your name>",
												"<server's domain name>",
												"<IP address>",
												"<port number>",
												"<current date and time>",
												"<date and time at which the object was created>"};	//original string
		char *replace_string[REPLACE_SIZE] = {	MY_NAME,
												getLocalDomainName(),
												getLocalIPAddress(),
												PORT_NUMBER,
												convertTimeFormat(getTimeInGMTFormat(url, 0), 1),
												convertTimeFormat(getTimeInGMTFormat(url, 1), 1)};	//replace string
		//Replace the String <your name>, <server's domain name>, ...
		for (i = 0; i < REPLACE_SIZE; i++){
			file_length = replaceString(content, file_length, original_string[i], replace_string[i]);
		}
		
	}
	
	return file_length;
}
