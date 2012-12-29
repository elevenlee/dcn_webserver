////////////////////////////////////////////////////////////////////////////////////
/*
	File Name:		DieWithMessage.c
	Instructor:		Prof. Arthur Goldberg
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This DieWithMessage.c file includes
					User Error Message Function and System Error Message Function.
*/
////////////////////////////////////////////////////////////////////////////////////

///////////////HEADER FILES///////////////
#include "Client.h"

///////////////FUNCTIONS///////////////
/*User Error Message Function
  Variable Definition:
  -- message: summary of error message
  -- detail: detail error message based on error code
  Return value: NULL
*/
void dieWithUserMessage(const char *message, const char *detail){
	DEBUG_PRINT;
	fputs(message, stderr);
	fputs(": ", stderr);
	fputs(detail, stderr);
	fputc('\n', stderr);
	exit(1);
}

/*System Error Message Function
  Variable Definition:
  -- message: summary of error message
  Return value: NULL
*/
void dieWithSystemMessage(const char *message){
	DEBUG_PRINT;
	perror(message);
	exit(1);
}
