/* Student Name: KaptainJosh
 * Description: This program runs the server side of a UDP connection. It simulates a UDP connection by randomly dropping packets it recieves. 
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/ioctl.h>
#include <net/if.h>

int main(int argc, char** argv)
{
	if (argc == 2) // if there are two arguments continue
	{
		int socket_desc; // for the socket
		int port = atoi(argv[1]); //holds port number
		struct sockaddr_in server_addr,client_addr; // hold server and client address information
		char server_message[2000], client_message[2000]; // hold the message from client and server
		int client_struct_length = sizeof(client_addr);

		// Clean buffers
		memset(server_message, '\0', sizeof(server_message));
		memset(client_message, '\0', sizeof(client_message));

		socket_desc = socket(AF_INET, SOCK_DGRAM, 0 ); // Create UDP socket

		if (socket_desc < 0)
		{
			puts("Error while creating socket");
			return -1;
		}
		
		// This section obtains the eth0 ip address of the machine
		struct ifreq ifc;
		int res;

		strcpy(ifc.ifr_name, "eth0");
		res = ioctl(socket_desc, SIOCGIFADDR, &ifc);
		if (res < 0)
		{
			puts("Failed");
		}
		char ip[16]; // holds the ip address of eth0 interface
		
		strcpy(ip, inet_ntoa(((struct sockaddr_in*) &ifc.ifr_addr)->sin_addr));
		// Set port and IP
		server_addr.sin_family = AF_INET;
    		server_addr.sin_port = htons(port);
		server_addr.sin_addr.s_addr = inet_addr(ip);

		// Bind to the set port and IP
		if (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
		{
        		printf("Couldn't bind to the port\n");
        		return -1;
    		}

		puts("[server]: ready to accept data...");

		srand(time(0)); // seed random number
		int random; // holds the random number
		while(1)
		{
			//Recieve client's message
			if (recvfrom(socket_desc, client_message, sizeof(client_message), 0,(struct sockaddr*)&client_addr, &client_struct_length) < 0)
			{
				printf("Couldn't receive\n");
				return -1;
			}

			random = rand() % 3; //setting random number between 0 and 2

			if (random == 0) // if 0 then drop packet
			{
				puts("[server]: dropped packet");
			}

			else // else respond
			{
				printf("[client]: %s\n", client_message);

				strcpy(server_message, "PONG");

				// sending message back to client
				if (sendto(socket_desc, server_message, strlen(server_message), 0,(struct sockaddr*)&client_addr, client_struct_length) < 0)
				{
					printf("Can't send\n");
					return -1;
				}
			}
		}

	}// if

	else // if not right amount of arguments, print usage statement
	{
		puts("usage: ./svr <port>");
	}

	return 0;
}
