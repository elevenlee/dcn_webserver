////////////////////////////////////////////////////////////
/*
	File Name:		AddressUtility.c
	Instructor:		Prof. Arthur Goldberg
	Author:			Shen Li
	UID:			N14361265
	Department:		Computer Science
	Note:			This AddressUtility.c file includes 
					Handle Socket Address Functions.
*/
////////////////////////////////////////////////////////////

///////////////HEADER FILES///////////////
#include "Server.h"

///////////////FUNCTIONS///////////////
/*Print Socket Address Function
  Variable Definition:
  -- address: socket address structure
  -- stream: file stream(stdin, stdout, stderr, or file objective...)
  Return Value: NULL
*/
void printSocketAddress(const struct sockaddr *address, FILE *stream){
	void 		*numeric_address;					//pointer to binary address
	char 		address_buffer[INET6_ADDRSTRLEN];	//address data
	in_port_t 	port;								//port number
	
	//Test for address and stream
	if (address == NULL || stream == NULL){
		return;
	}
	
	//Set pointer to address based on address family
	switch (address->sa_family){
		//The type of address is ipv4
		case AF_INET:
			numeric_address = &((struct sockaddr_in*)address)->sin_addr;
			port = ntohs(((struct sockaddr_in*)address)->sin_port);
			break;
		//The type of address is ipv6
		case AF_INET6:
			numeric_address = &((struct sockaddr_in6*)address)->sin6_addr;
			port = ntohs(((struct sockaddr_in6*)address)->sin6_port);
			break;
		//Invalid address
		default:
			fputs("[unknown type]", stream);
			return;
	}
	
	//Convert binary to printable address
	if (inet_ntop(address->sa_family, numeric_address, address_buffer, sizeof(address_buffer)) == NULL){
		//Unable to convert
		fputs("[invalid address]", stream);
	}
	else{
		//Output the address
		fprintf(stream, "%s", address_buffer);
		if (port != 0){
			//Output the port number
			fprintf(stream, "-%u", port);
		}
	}
}

/*Get Local IP Address Function
  Variable Definition: NULL
  Return Value: local ip address string
*/
char *getLocalIPAddress(){
	char 			buffer[HALFBUF_SIZE];	//buffer
	char 			*ip_address;			//ip address
	int 			i = 0;					//counter
	int 			local_socket;			//socket descriptor for local
	struct ifconf 	if_conf;				//network interface information
	struct ifreq 	*if_req;				//ip address structure
	
	//Initialize the ifconf structure
	if_conf.ifc_len = HALFBUF_SIZE;
	if_conf.ifc_buf = buffer;
	
	//Create socket for getting lcoal ip address
	if ((local_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		dieWithSystemMessage("socket() failed!");
	}
	//Get all network interface information
	ioctl(local_socket, SIOCGIFCONF, &if_conf);
	//Close local socket
	close(local_socket);
	
	//Get all ip address
	if_req = (struct ifreq*)buffer;
	for (i = (if_conf.ifc_len/sizeof(struct ifreq)); i > 0; i--){
		ip_address = inet_ntoa(((struct sockaddr_in*)&(if_req->ifr_addr))->sin_addr);
		//Do not get local 127.0.0.1 address
		if (strcmp(ip_address, LOCAL_ADDRESS) == 0){
			if_req++;
			continue;
		}
		return ip_address;
	}
	
	return LOCAL_ADDRESS;
}

/*Get Local Domain Name Function
  Variable Definition: NULL
  Return Value: local domain name string
*/
char *getLocalDomainName(){
	char	*domain_name = (char*)malloc(sizeof(char)*(STRING_SIZE + 1));	//domain name
	
	//Get Local Host Name
	if (gethostname(domain_name, STRING_SIZE + 1) == -1){
		dieWithSystemMessage("gethostname() failed!");
	}
	
	return domain_name;
}
