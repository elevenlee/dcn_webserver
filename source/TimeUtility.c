////////////////////////////////////////////////////////////////////
/*
	File Name:		TimeUtility.c
	Instructor:		Prof. Arthur Goldberg
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This TimeUtility.c file includes 
					Get GMT time and Convert Time Format Functions.
*/
////////////////////////////////////////////////////////////////////

///////////////HEADER FILES///////////////
#include "Server.h"

///////////////FUNCTIONS///////////////
/*Get GMT Time Function (including System time and File time)
  Variable Definition:
  -- url: the request url except domain name and port number
  -- signal_value: signal that decide which kind of time needed
  Return value: tm struct in GMT Format
*/
struct tm *getTimeInGMTFormat(char *url, int signal_value){
	struct stat		file_information;	//file information sstructure
	time_t			t;					//time structure
	
	//signal_value equals to 0, get the system current time
	if (!signal_value){
		time(&t);
	}
	//signal_value not equals to 0, get the file time(Create time, Modify time, Access time...)
	else if (stat(url, &file_information) != -1){
		switch(signal_value){
			//signal_value is 1, get the file create time
			case 1:	t = file_information.st_atime;	break;
			//signal_value is 2, get the file modify time
			case 2:	t = file_information.st_mtime;	break;
			//signal_value is others
			default:								break;
		}
	}
	//Cannot find the file information
	else{
		dieWithUserMessage("stat() failed! Cannot find the file information, file name", url);
	}
	
	return gmtime(&t);
}

/*Convert Time Format to a string
  Variable Definition:
  -- gmt_time: tm struct in GMT format
  -- signal_value: signal that decide which time format to convert
  Return value: time string in GMT format
*/
char *convertTimeFormat(struct tm *gmt_time, int signal_value){
	char *gmt_time_string = (char*)malloc(sizeof(char) * (TIME_SIZE + 1));	//time in GMT format string

	//According to the signal_value, convert time to different format 
	switch(signal_value){
		case 1:
			strftime(gmt_time_string, TIME_SIZE, "%a, %d %b %Y %H:%M:%S GMT", gmt_time);
			break;
		case 2:
			strftime(gmt_time_string, TIME_SIZE, "%A, %d-%b-%y %H:%M:%S GMT", gmt_time);
			break;
		case 3:
			gmt_time_string = asctime(gmt_time);
			gmt_time_string[strlen(gmt_time_string) - 1] = '\0';
			break;
		default:
			break;
	}
	
	return gmt_time_string;
}

/*Compare the If-Modified-Since field and Last-Modified field Function
  Variable Definition:
  -- url: the request url except domain name and port number
  -- modified_time_string: If-Modified-Since field value
  Return Value: if If-Modified-Since field equals to Last-Modified field, return 1; else return 0
*/
bool compareModifiedTime(char *url, char *modified_time_string){
	struct tm	*file_modified_time = getTimeInGMTFormat(url, 2);	//tm struct with the file last modified time
	int			i;													//counter
	
	//Test the modified time is equal(three format: RFC 1123, RFC 1036, and ANSI C's format)
	for (i = 1; i < NUMBER_SIZE; i++){
		if (strcmp(modified_time_string, convertTimeFormat(file_modified_time, i)) == 0){
			return true;
		}
	}
	return false;
}
