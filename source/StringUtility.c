////////////////////////////////////////////////////////////
/*
	File Name:		StringUtility.c
	Instructor:		Prof. Arthur Goldberg
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This StringUtility.c file includes 
					Handle String Functions.
*/
////////////////////////////////////////////////////////////

///////////////HEADER FILES///////////////
#include "Server.h"

///////////////FUNCTIONS///////////////
/*Convert int type to char* type Function
  Variable Definition:
  -- number: original number
  Return Value: convert string
*/
char *itoa(int number){
	int		i = number;													//number
	int		j;															//counter
	int		power;														//power
	char	*string = (char*)malloc(sizeof(char) * (NUMBER_SIZE + 1));	//conver string
	
	//Find the max power
	for (power = 1, j = 1; i >= 10; i /= 10, j++){
		power *= 10;
	}
	//Get the string which is converted by number
	for (; power > 0; power /= 10){
		*string++ = '0' + number / power;
		number %= power;
	}
	//Set the end of the string
	*string = '\0';

	return string - j;
}

/*Decode %HEX Format URL Function
  Variable Definition:
  -- url: the request url except domain name and port number
  Return Value: NULL
*/
void decodeURL(char *url){
	char	*source = NULL;		//source string
	int		number;				//number(char) represent original %HEX

	//Initilize the source by given url
	source = url;
	//Test the %HEX in url variable
	while (*source){
		if (*source == '%'){
			source++;
			//Convert HEX to number(char)
			sscanf(source, "%2x", &number);
			*url++ = number;
			//Jump the %HEX number
			source += 2;
		}
		else{
			*url++ = *source++;
		}
	}
	//Set the end of the url
	*url = '\0';
}

/*Get paths below the current directory Function
  Variable Definition:
  -- url: the request url except domain name and port number
  Return Value: NULL
*/
void pathBelowCurrentDirectory(char *url){
	char *source = NULL;		//source string
	char *destination = NULL;	//destination string
	
	//Initilize the source and destination by given url
	source = destination = url;
	//Test whether the url contains "/../" and "//"
	while (*source){
		//If contain "/../", remove it
		if (strncmp(source, "/../", 4) == 0){
			source += 3;
		}
		//if contain "//", remove it
		else if (strncmp(source, "//", 2) == 0){
			source++;
		}
		else{
			*destination++ = *source++;
		}
	}
	//Set the end of the destination string
	*destination = '\0';
	//Remove the first '/' in url
	if (*url == '/'){
		strcpy(url, url + 1);
	}
	//If the url is the root directory, set url as '.'
	if (url[0] == '\0' || strcmp(url, "./") == 0 || strcmp(url, "./..") == 0){
		strcpy(url, ".");
	}
}

/*Check Header Lines Function
  Variable Definition:
  -- header: client header lines
  -- field_value: syntax incorrect header line string
  Return Value: if all header lines are syntactically incorrect, return 1; else return 0
*/
bool headerLinesIncorrect(struct http_header *header, char *field_value){
	struct http_header *node;	//http_header structure node

	for (node = header->next; node != NULL; node = node->next){
		//Use regular expression to check header lines' syntax
		if (!syntaxChecking(node->header_line, 'H')){
			sprintf(field_value, "Header line: [%s] is syntacically incorrect!", node->header_line);
			return true;
		}
	}

	return false;
}

/*Test URL is not exist Function
  Variable Definition:
  -- url: the request url except domain name and port number
  Return Value: if the url is not exist, return 0; else, return 1
*/
bool urlNotExist(char *url){
	struct stat file_information;	//file information structure
	
	return (stat(url, &file_information) == -1);
}

/*Test Header Field is not exist Function
  Variable Definition:
  -- header: http_header structure head pointer
  -- field_name: field name
  -- field_value: field value
  Return Value: if field exist, return 0; else return 1
*/
bool fieldNotExist(struct http_header *header, const char *field_name, char *field_value){
	struct http_header *node;	//http_header structure node

	// NC: GOOD, but need to compare without concern for case; #23: -2
	//Initialize node pointer
	node = header;
	//Find the field name
	while (node != NULL){
		if (strcmp(node->field_name, field_name) != 0){
			node = node->next;
		}
		else{
			break;
		}
	}
	//Get the field value
	if (node != NULL){
		strcpy(field_value, node->field_value);
	}
	
	return (node == NULL);
}

/*Test URL is a directory Function
  Variable Definition:
  -- url: the request url except domain name and port number
  Return Value: if the url is a directory, return 1; else return 0
*/
bool urlIsADirectory(char *url){
	struct stat file_information;	//file information structure
	
	return (stat(url, &file_information) != -1 && S_ISDIR(file_information.st_mode));
}

/*Split the Name and Value of Header Line Function
  Variable Definition:
  -- header_line: client request header lines
  -- stop: split character
  Return Value: name of the header_line (before the stop of the header_line)
*/
char *splitNameAndValue(char *header_line, const char stop){
	int		i = 0;															//counter for name
	int 	j = 0;															//counter for value
	char	*name = (char*)malloc(sizeof(char)*(strlen(header_line) + 1));	//name string
	
	//Jump the ' ' or Tab character(blank character)
	for (i = 0; isspace(header_line[i]); i++){
		;	
	}
	//Set the name in the header line
	for (j = 0; header_line[i] && (header_line[i] != stop); i++, j++){
		//Lowercase the field name
		name[j] = tolower(header_line[i]);
	}
	//Set the end of the name string
	name[j] = '\0';
	
	//Test whether has more characters after name
	if (header_line[i]){
		//Jump the "stop" character
		++i;
		//Jump the ' ' or Tab character(blank chacater)
		for (; isspace(header_line[i]); i++){
			;
		}
	}
	//Set the value in the header line and remove the '\r' and '\n' character
	for (j = 0; header_line[i] && (header_line[i] != '\r') && (header_line[i] != '\n'); i++, j++){
		header_line[j] = header_line[i];
	}
	//Set the end of the value string
	header_line[j] = '\0';
	
	return name;
}

/*Replace String Function
  Variable Definition:
  -- content: original total string
  -- length: total string length
  -- original: string which is original
  -- replace: string which used to replace
  Return Value: total string length after replaced by new string
*/
int	replaceString(char *content, int length, const char *original, const char *replace){
	int		i = 0;						//counter
	int 	j = 0;						//counter
	int		k = 0;						//counter
	int		m = 0;						//counter
	int		position = 0;				//position
	char	new_content[FILE_SIZE];		//total string after replacee by new stirng
	
	//Initialize the new_content buffer
	memset(new_content, 0, FILE_SIZE);
	//Find the first character of original string
	for (i = 0; i < length - strlen(original); i++){
		//Find the next characters of original string
		for (j = 0; j < strlen(original) && (content[i + j] == original[j]); j++){
			;
		}
		//If not the original string, then continue the "for" loop
		if (j != strlen(original)){
			continue;
		}
		//Have found the original string
		else{
			//Get the position of the original string
			for (k = 0; k < i - position; k++){
				//Copy the characters which are no need to replace
				new_content[m++] = content[position + k];
			}
			//Get the position
			position = i + strlen(original);
			//Replace the original string
			for (k = 0; k < strlen(replace); k++){
				new_content[m++] = replace[k];
			}
		}
	}
	//Copy the rest characters of the content
	for (k = position; k < length; k++){
		new_content[m++] = content[k];
	}
	//Update the content
	strcpy(content, new_content);
	
	return m;
}

/*Use Regular Expressions to check Request Message Function
  Variable Definition:
  -- string: string need to be checked
  -- signal_value: signal that decide the checking type
  Return Value: if matched, return 1; else return 0
*/
bool syntaxChecking(char *string, const char signal_value){
	char 		*pattern = (char*)malloc(sizeof(char) * (DOUBUF_SIZE));			//regular expression string
	char		*error_buffer = (char*)malloc(sizeof(char) * (STRING_SIZE));	//error buffer
	// NC: what if malloc returns null?; #12, -1
	int 		status;															//result status
	size_t 		nmatch = ONE_SIZE;												//max number of match result
	regex_t 	reg;															//regex_t structure
	regmatch_t	pmatch[ONE_SIZE];												//match result
	
	// NC: cool use of REs, but the signal_values are funny codes; can C switch on enumerations?
	// 
	//According to the signal value, determine using which regular expression
	switch(signal_value){
		//Blank line
		case 'B':
			pattern = "^[\t| ]*\r\n$";
			break;
		//Request line
			// NC: based on http://pubs.opengroup.org/onlinepubs/009695399/basedefs/xbd_chap09.html#tag_09_04_03
			// this seems to match more than the 5.1 Request-Line as defined in 2616:
			// Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
			// #13: -2
			// no tabs, no leading tabs (is \t a tab?), certainly not an arbitrary number, 
			// also, does it do capture groups? it's a shame to go to all this RE trouble, and then not capture the fields you want
			
			// 
		case 'R':
			pattern = "^[\t| ]*[^\t| ]+[ ][^\t| ]+[ ][^\t| ]+\r\n$";
			break;
		//URL format
		case 'U':
			pattern = "(/[A-Z0-9a-z_!~*'().;?:@&=+$,%#-]*)+/?$";
			break;
		//HTTP version format
		case 'V':
			pattern = "^HTTP/[0-9]+[.][0-9]+$";
			break;
		//HTTP version 1.1
		case 'O':
			pattern = "^HTTP/[0]*[1][.][0]*[1]$";
			break;
		//Header lines
		case 'H':
			pattern = "^[\t| ]*[a-zA-Z-]+:(([\t| ]*)|(.+[^\t| ]))\r\n$";
			break;
		//Host field value
		case 'I':
			pattern = 	"^(((2[0-4][0-9]|25[0-5]|[01]?[0-9][0-9]?)[.]){3}(2[0-4][0-9]|25[0-5]|[01]?[0-9][0-9]?)"	\
						"|([0-9a-z_!~*'()-]+[.])*([0-9a-z][0-9a-z-]{0,61})?[0-9a-z][.][a-z]{2,6}"	\
						"|(localhost))"	\
						"(:([0-9]|[1-9][0-9]|[1-9][0-9]{2}|[1-9][0-9]{3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5]))?$";
			break;
		//Time format(three kinds of format)
		case 'T':
			pattern = 	"^((Sun|Mon|Tue|Wed|Thu|Fri|Sat),"	\
						"[ ]((((31[ ](Jan|Mar|May|Jul|Aug|Oct|Dec))"	\
						"|((0[1-9]|[12][0-9]|30)[ ](Jan|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec))"	\
						"|((0[1-9]|1[0-9]|2[0-8])[ ]Feb))[ ]([0-9]{4}))"	\
						"|(29[ ]Feb[ ]([0-9]{2}(0[48]|[2468][048]|[13579][26])|(0[48]|[2468][048]|[13579][26])00)))"	\
						"[ ](([01][0-9]|2[0-3]):([0-5][0-9]):([0-5][0-9]))[ ]GMT)"	\
						"|((Sunday|Monday|Tuesday|Wednesday|Thursday|Friday|Saturday),"	\
						"[ ]((((31-(Jan|Mar|May|Jul|Aug|Oct|Dec))"	\
						"|((0[1-9]|[12][0-9]|30)-(Jan|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec))"	\
						"|((0[1-9]|1[0-9]|2[0-8])-Feb))-([0-9]{2}))"	\
						"|(29-Feb-(0[048]|[2468][048]|[13579][26])))"	\
						"[ ](([01][0-9]|2[0-3]):([0-5][0-9]):([0-5][0-9]))[ ]GMT)"	\
						"|((Sun|Mon|Tue|Wed|Thu|Fri|Sat)"	\
						"[ ](((((Jan|Mar|May|Jul|Aug|Oct|Dec)[ ]31)"	\
						"|((Jan|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)[ ]([ ][1-9]|[12][0-9]|30))"	\
						"|(Feb[ ]([ ][1-9]|1[0-9]|2[0-8])))"	\
						"[ ](([01][0-9]|2[0-3]):([0-5][0-9]):([0-5][0-9]))"	\
						"[ ]([0-9]{4}))"	\
						"|(Feb[ ]29[ ](([01][0-9]|2[0-3]):([0-5][0-9]):([0-5][0-9]))"	\
						"[ ]([0-9]{2}(0[48]|[2468][048]|[13579][26])|(0[48]|[2468][048]|[13579][26])00))))$";
			break;
		default:
			pattern = "";
			break;
	}
	//Compile the regular expression
	status = regcomp(&reg, pattern, REG_EXTENDED);
	if (status != 0){
		regerror(status, &reg, error_buffer, STRING_SIZE);
		dieWithUserMessage("regcomp() failed!", error_buffer);
	}
	//Match the regular expression
	status = regexec(&reg, string, nmatch, pmatch, 0);
	//If there is no match result, return 0
	if (status == REG_NOMATCH){
		return false;
	}
	//Close the regex_t structure
	regfree(&reg);
	
	return true;
}
