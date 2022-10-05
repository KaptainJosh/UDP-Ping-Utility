/* Author: KaptainJosh
 * Description: This program is the client side of a UDP connection. It sends UDP packets to a server program and creates statistics based on the connection.
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
#include <sys/time.h>

int main(int argc, char** argv)
{
	if (argc == 3) // if right amount of arguments, continue
	{
		int socket_desc; // for socket
		int port = atoi(argv[2]); // port number
		struct sockaddr_in server_addr; // server address
		char server_message[2000]; // server message
		char client_message[2000]; // client message
		int server_struct_length = sizeof(server_addr); 

		// Clean buffers
		memset(server_message, '\0', sizeof(server_message));
		memset(client_message, '\0', sizeof(client_message));

		socket_desc = socket(AF_INET, SOCK_DGRAM, 0 ); // Create UDP socket

		if (socket_desc < 0)
		{
			puts("Error while creating socket");
			return -1;
		}

		//Get hostname and ip address
		char* hostbuffer = argv[1];
		char *IPbuffer;
		struct hostent *host_entry;

		host_entry = gethostbyname(hostbuffer); // Getting info on this hostname

		IPbuffer = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); // Getting IP address of the hostname

		// Set port and IP
		server_addr.sin_family = AF_INET;
    		server_addr.sin_port = htons(port);
    		server_addr.sin_addr.s_addr = inet_addr(IPbuffer);

		// this struct is used to set how long the socket should wait before timeout
		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		
		struct timeval timestart; // this struct represents the time of sending packet to server
		struct timeval timeend; // this struct represents the time of recieving packet from server
		double rtt; // the round trip time for an individual interaction
		double rttArray[10]; // collection of all rtts

		int packetsSent = 0; // tracks how many packets are sent
		int packetsRecieved = 0; // tracks how many packets are recieved
		strcpy(client_message, "PING"); // setting client message

		for (int i = 0; i < 10; i++) // this loop sends 10 UDP messages
		{
			gettimeofday(&timestart, NULL); // getting time of sending
			// Send message to server
			if (sendto(socket_desc, client_message, strlen(client_message), 0, (struct sockaddr*)&server_addr, server_struct_length) < 0) // sending packet
			{
				printf("Unable to send message\n");
				return -1;
			}
			packetsSent++;
			printf("%2d: Sent... ", i+1);

			setsockopt(socket_desc, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)); // setting timout time for socket

			// Receive the server's response
    			if (recvfrom(socket_desc, server_message, sizeof(server_message), 0, (struct sockaddr*)&server_addr, &server_struct_length) < 0) // recieving message
			{
				printf("Timed Out\n");
			}

			else // if message is recieved make stats
			{
				gettimeofday(&timeend, NULL); // time packet is recieved
				rtt = (timeend.tv_sec - timestart.tv_sec) * 1000.0 + (timeend.tv_usec - timestart.tv_usec) / 1000.0; // calculate rtt

				printf("RTT=%lf ms\n", rtt);
				rttArray[packetsRecieved] = rtt; // save rtt to collection
				packetsRecieved++;
			}
		}

		close(socket_desc); // close socket

		int percentLoss = (packetsSent - packetsRecieved) * 10; // percent of packets lost
		printf("%d packets sent, %d packets recieved, %d%% packet loss\n", packetsSent, packetsRecieved, percentLoss); // packet stats

		double minRtt = rttArray[0]; // mininum RTT
                double maxRtt = rttArray[0]; // maximum RTT
                double avgRtt; // average RTT

		// find minimum RTT
		for (int j = 1; j < packetsRecieved; j++)
		{
			if (rttArray[j] < minRtt)
			{
				minRtt = rttArray[j];
			}	
		}

		// find maximum RTT
		for (int k = 1; k < packetsRecieved; k++)
		{
			if (rttArray[k] > maxRtt)
			{
				maxRtt = rttArray[k];
			}
		}

		// Below calculate average RTT
		double sum = rttArray[0];
		for (int l = 1; l < packetsRecieved; l++)
		{
			sum += rttArray[l];
		}

		avgRtt = sum / (double)packetsRecieved;

		printf("min: %lf ms, max: %lf ms, avg: %lf ms\n", minRtt, maxRtt, avgRtt); // print out RTT stats

	}// if

	else // else print out usage statement
	{
		puts("usage: ./cli <hostname> <port>");
	}

	return 0;
}
