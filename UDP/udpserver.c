#ifdef _WIN32
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <winsock2.h> //for all socket programming
#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
#include <stdio.h>	  //for fprintf, perror
#include <unistd.h>	  //for close
#include <stdlib.h>	  //for exit
#include <string.h>	  //for memset
#include<time.h> //for timeout
void OSInit(void)
{
	WSADATA wsaData;
	int WSAError = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if (WSAError != 0)
	{
		fprintf(stderr, "WSAStartup errno = %d\n", WSAError);
		exit(-1);
	}
}
void OSCleanup(void)
{
	WSACleanup();
}
#define perror(string) fprintf(stderr, string ": WSA errno = %d\n", WSAGetLastError())
#else
#include <sys/socket.h> //for sockaddr, socket, socket
#include <sys/types.h>	//for size_t
#include <netdb.h>		//for getaddrinfo
#include <netinet/in.h> //for sockaddr_in
#include <arpa/inet.h>	//for htons, htonl, inet_pton, inet_ntop
#include <errno.h>		//for errno
#include <stdio.h>		//for fprintf, perror
#include <unistd.h>		//for close
#include <stdlib.h>		//for exit
#include <string.h>		//for memset
int OSInit(void) {}
int OSCleanup(void) {}
#endif

int initialization();
void execution(int internet_socket);
void cleanup(int internet_socket);
int package_expected = 0;
int package_arive = 0;
int timeout = 0;
clock_t timer;
int timeouts = 10;
FILE *fptr;
FILE *OUTPUTFILESTATS;
double rommel[5] = {0,0,0,0,0,0};
double stats1[2] = {0,0,0};
double stats2[2] = {0,0,0};
double stats3[2] = {0,0,0};
double maxstat1[2] = {-__DBL_MAX__,-__DBL_MAX__,-__DBL_MAX__};
double maxstat2[2] = {-__DBL_MAX__,-__DBL_MAX__,-__DBL_MAX__};
double maxstat3[2] = {-__DBL_MAX__,-__DBL_MAX__,-__DBL_MAX__};
double minstat1[2] = {__DBL_MAX__,__DBL_MAX__,__DBL_MAX__};
double minstat2[2] = {__DBL_MAX__,__DBL_MAX__,__DBL_MAX__};
double minstat3[2] = {__DBL_MAX__,__DBL_MAX__,__DBL_MAX__};
double statsavg1 [2] = {0,0,0};
double statsavg2 [2] = {0,0,0};
double statsavg3 [2] = {0,0,0};


int main(int argc, char *argv[])
{
	//////////////////
	// Initialization//
	//////////////////

	OSInit();

	int internet_socket = initialization();

	/////////////
	// Execution//
	/////////////
	
	printf("hoeveel pakketten worden er verwacht : ");
	scanf("%i",&package_expected);
	printf("hoeveel seconden voor een time out : ");
	scanf("%i",&timeouts);
	timeouts = timeouts * CLOCKS_PER_SEC;
	do
	{
		execution(internet_socket);
	} while (package_arive < package_expected && timeout == 0 );
	timer = clock() - timer;
	double time_taken = ((double)timer)/CLOCKS_PER_SEC;
	printf("stats : \n expected packages : %i \n packages arrived : %i \n packages dropped: %i \n time taken : %lf seconds", package_expected, package_arive,package_expected - package_arive, time_taken);
	fprintf(OUTPUTFILESTATS,"The max values:\n");
		for (int i = 0; i < 3; i++)		
		{
			fprintf(OUTPUTFILESTATS,"%lf, ",maxstat1[i]);
		}
		fprintf(OUTPUTFILESTATS,"\nThe min values:\n");
		for (int i = 0; i < 3; i++)		
		{
			fprintf(OUTPUTFILESTATS,"%lf, ",minstat1[i]);
		}
		fprintf(OUTPUTFILESTATS,"\nThe average values:\n");
		for (int i = 0; i < 3; i++)		
		{
			fprintf(OUTPUTFILESTATS,"%lf, ",statsavg1[i]);
		}

		fprintf(OUTPUTFILESTATS,"\n\nBellow you find the parsed values for stats2:\n");
		fprintf(OUTPUTFILESTATS,"The max values:\n");
		for (int i = 0; i < 3; i++)		
		{
			fprintf(OUTPUTFILESTATS,"%lf, ",maxstat2[i]);
		}
		fprintf(OUTPUTFILESTATS,"\nThe min values:\n");
		for (int i = 0; i < 3; i++)		
		{
			fprintf(OUTPUTFILESTATS,"%lf, ",minstat2[i]);
		}
		fprintf(OUTPUTFILESTATS,"\nThe average values:\n");
		for (int i = 0; i < 3; i++)		
		{
			fprintf(OUTPUTFILESTATS,"%lf, ",statsavg2[i]);
		}
		
		fprintf(OUTPUTFILESTATS,"\n\nBellow you find the parsed values for stats3:\n");
		fprintf(OUTPUTFILESTATS,"The max values:\n");
		for (int i = 0; i < 3; i++)		
		{
			fprintf(OUTPUTFILESTATS,"%lf, ",maxstat3[i]);
		}
		fprintf(OUTPUTFILESTATS,"\nThe min values:\n");
		for (int i = 0; i < 3; i++)		
		{
			fprintf(OUTPUTFILESTATS,"%lf, ",minstat3[i]);
		}
		fprintf(OUTPUTFILESTATS,"\nThe average values:\n");
		for (int i = 0; i < 3; i++)		
		{
			fprintf(OUTPUTFILESTATS,"%lf, ",statsavg3[i]);
		}
	////////////
	// Clean up//
	////////////

	cleanup(internet_socket);

	OSCleanup();

	return 0;
}

int initialization()
{
	fptr = fopen("Datastream.csv", "a");
    OUTPUTFILESTATS = fopen("data.csv", "a");
	// Step 1.1
	struct addrinfo internet_address_setup;
	struct addrinfo *internet_address_result;
	memset(&internet_address_setup, 0, sizeof internet_address_setup);
	internet_address_setup.ai_family = AF_INET;
	internet_address_setup.ai_socktype = SOCK_DGRAM;
	internet_address_setup.ai_flags = AI_PASSIVE;
	int getaddrinfo_return = getaddrinfo(NULL, "24042", &internet_address_setup, &internet_address_result);
	if (getaddrinfo_return != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfo_return));
		exit(1);
	}

	int internet_socket = -1;
	struct addrinfo *internet_address_result_iterator = internet_address_result;
	while (internet_address_result_iterator != NULL)
	{
		// Step 1.2
		
		internet_socket = socket(internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol);
		if (internet_socket == -1)
		{
			perror("socket");
		}

		else
		{
			// Step 1.3
			int bind_return = bind(internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen);
			if (bind_return == -1)
			{
				close(internet_socket);
				perror("bind");
			}
			else
			{
				break;
			}
		}
		internet_address_result_iterator = internet_address_result_iterator->ai_next;
	}

	freeaddrinfo(internet_address_result);

	if (internet_socket == -1)
	{
		fprintf(stderr, "socket: no valid socket address found\n");
		exit(2);
	}
	return internet_socket;
}

void execution(int internet_socket)
{
	// Step 2.1
	int number_of_bytes_received = 0;
	char buffer[1000];
	struct sockaddr_storage client_internet_address;
	socklen_t client_internet_address_length = sizeof client_internet_address;
	number_of_bytes_received = recvfrom(internet_socket, buffer, (sizeof buffer) - 1, 0, (struct sockaddr *)&client_internet_address, &client_internet_address_length);
	if (number_of_bytes_received == -1)
	{
		perror("recvfrom");
		timeout = 1; 
	}
	else if(setsockopt(internet_socket, SOL_SOCKET, SO_RCVTIMEO,&timeouts,sizeof(timeouts)) < 0) 
	{
        perror("Error");
	}
	else
	{
		
		

		buffer[number_of_bytes_received] = '\0';
		printf("Received : %s\n", buffer);
		if (package_arive == 0)
		{
			timer = clock();
		}
		package_arive++;
		
    if (fptr == NULL)
    {
        printf("Cannot open file \n");
        exit(0);
    }
	sscanf(buffer,"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",&rommel,&rommel,&stats1[0],&stats1[1],&stats1[2],&rommel,&stats2[0],&stats2[1],&stats2[2],&rommel,&stats3[0],&stats3[1],&stats3[2]);
	//Gets the max for stats1-3.
				for (size_t i = 0; i < 3; i++)
				{
					if (stats1[i] > maxstat1[i])
					{
						maxstat1[i] = stats1[i];
					}

					if (stats2[i] > maxstat2[i])
					{
						maxstat2[i] = stats2[i];
					}

					if (stats3[i] > maxstat3[i])
					{
						maxstat3[i] = stats3[i];
					}

				}

				//Gets the min for stats1-3.
				for (size_t i = 0; i < 3; i++)
				{
					if (stats1[i] < minstat1[i])
					{
						minstat1[i] = stats1[i];
					}

					if (stats2[i] < minstat2[i])
					{
						minstat2[i] = stats2[i];
					}
					
					if (stats3[i] < minstat3[i])
					{
						minstat3[i] = stats3[i];
					}
		
				}
				for (size_t i = 0; i < 3; i++)
				{
					statsavg1[i] = statsavg1[i] + stats1[i];
					statsavg2[i] = statsavg2[i] + stats2[i];
					statsavg3[i] = statsavg3[i] + stats3[i];
				}
				for (size_t i = 0; i < 3; i++)
		{
			statsavg1[i] = statsavg1[i] / (package_expected -package_arive);
			statsavg2[i] = statsavg1[i] / (package_expected -package_arive);
			statsavg3[i] = statsavg1[i] / (package_expected -package_arive);
		}
			}
	fprintf(fptr,"%s\n",buffer);

}


void cleanup(int internet_socket)
{
	// Step 3.1
	close(internet_socket);
}