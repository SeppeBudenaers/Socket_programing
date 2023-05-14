#ifdef _WIN32
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <winsock2.h> //for all socket programming
#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
#include <stdio.h>	  //for fprintf, perror
#include <unistd.h>	  //for close
#include <stdlib.h>	  //for exit
#include <string.h>	  //for memset
#include <time.h>	  //for timeout
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
void OSInit(void) {}
void OSCleanup(void) {}
#endif

int initialization();
void execution(int internet_socket);
void cleanup(int internet_socket);
int initializationudp(struct sockaddr **internet_address, socklen_t *internet_address_length);
void executionudp(int internet_socket, struct sockaddr *internet_address, socklen_t internet_address_length);
void cleanupudp(int internet_socket, struct sockaddr *internet_address);
int initializationserver();
int connectionserver( int internet_socket );
void executionserver( int internet_socket );
void cleanupserver( int internet_socket, int client_internet_socket );
char messagebuffer[1000];
char poort[10];
int trigger = 0;
int trigger1 = 0;
int bytebuffer = 0;
clock_t timer;
int timeouts = 500;
timeout = 0;
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

	recving(internet_socket);
	sprintf(messagebuffer, "{91508323-9825-42a2-a7f8-24cdb528b04d}");
	SEND(internet_socket);
	recving(internet_socket);
	recving(internet_socket);
	while (trigger == 0)
	{
		SEND(internet_socket);
		recving(internet_socket);
	}
	printf("out of while loop");

	////////////
	// Clean up//
	////////////

	cleanup(internet_socket);

	OSCleanup();
	////////////////////////////////////////////////////////////////////////////////// opdracht 1
	// Initialization

	OSInit();
	internet_socket = initialization();
	// Execution
	recving(internet_socket);
	do
	{
		sprintf(messagebuffer, "{91508323-9825-42a2-a7f8-24cdb528b04d}");
		SEND(internet_socket);

		recving(internet_socket);
		sprintf(poort, "%s", messagebuffer);
		recving(internet_socket);
		OSInit();
		printf("debug: %s en %s\n", poort, messagebuffer);
		struct sockaddr *internet_address = NULL;
		socklen_t internet_address_length = 0;
		int internet_socketudp = initializationudp(&internet_address, &internet_address_length);
		// Execution
		do
		{
			executionudp(internet_socketudp, internet_address, internet_address_length);
		} while (timeout == 0);

		// Clean up
		cleanupudp(internet_socketudp, internet_address);
		OSCleanup();

		////////////
		// Clean up//
		////////////
	} while (trigger1 == 0);
	SEND(internet_socket);
	cleanup(internet_socket);

	OSCleanup();

	/////////////////////////////////////////////////////////////////////opdracht 3
	//////////////////
	// Initialization//
	//////////////////

	OSInit();

	internet_socket = initialization();

	/////////////
	// Execution//
	/////////////

	recving(internet_socket);
	sprintf(messagebuffer,"24042");
	int server_socket = initializationserver();
	int client_internet_socket = connectionserver( server_socket );
	executionserver( client_internet_socket );
	cleanupserver( internet_socket, client_internet_socket );
	OSCleanup();


	////////////
	// Clean up//
	////////////

	cleanup(internet_socket);

	OSCleanup();
	return 0;
}

int initialization()
{
	// Step 1.1
	struct addrinfo internet_address_setup;
	struct addrinfo *internet_address_result;
	memset(&internet_address_setup, 0, sizeof internet_address_setup);
	// internet_address_setup.ai_family = AF_UNSPEC; //ipv6
	internet_address_setup.ai_family = AF_INET;																		   // ipv4
	internet_address_setup.ai_socktype = SOCK_STREAM;																   // tcp
	int getaddrinfo_return = getaddrinfo("192.168.1.142", "24040", &internet_address_setup, &internet_address_result); // "address","Poort"
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
			int connect_return = connect(internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen);
			if (connect_return == -1)
			{
				perror("connect");
				close(internet_socket);
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

void SEND(int internet_socket)
{
	// Step 2.1
	int number_of_bytes_send = 0;
	number_of_bytes_send = send(internet_socket, messagebuffer, sizeof(messagebuffer), 0);
	if (number_of_bytes_send == -1)
	{
		perror("send");
	}
}
void recving(int internet_socket)
{
	int number_of_bytes_received = 0;
	number_of_bytes_received = recv(internet_socket, messagebuffer, (sizeof messagebuffer) - 1, 0);
	if (number_of_bytes_received == -1)
	{
		perror("recv");
	}
	else if (number_of_bytes_received == 0)
	{
		trigger = 1;
	}
	else
	{
		messagebuffer[number_of_bytes_received] = '\0';
		printf("Received : %s\n", messagebuffer);
	}
}

void cleanup(int internet_socket)
{
	// Step 3.2
	int shutdown_return = shutdown(internet_socket, SD_SEND);
	if (shutdown_return == -1)
	{
		perror("shutdown");
	}

	// Step 3.1
	close(internet_socket);
}
int initializationudp(struct sockaddr **internet_address, socklen_t *internet_address_length)
{
	// Step 1.1
	struct addrinfo internet_address_setup;
	struct addrinfo *internet_address_result;
	memset(&internet_address_setup, 0, sizeof internet_address_setup);
	// internet_address_setup.ai_family = AF_UNSPEC; //ipv6
	internet_address_setup.ai_family = AF_INET;																		 // ipv4
	internet_address_setup.ai_socktype = SOCK_DGRAM;																 // udp
	int getaddrinfo_return = getaddrinfo("192.168.1.142", poort, &internet_address_setup, &internet_address_result); //"address","poort"
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
			*internet_address_length = internet_address_result_iterator->ai_addrlen;
			*internet_address = (struct sockaddr *)malloc(internet_address_result_iterator->ai_addrlen);
			memcpy(*internet_address, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen);
			break;
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

void executionudp(int internet_socket, struct sockaddr *internet_address, socklen_t internet_address_length)
{
	// Step 2.1
	int number_of_bytes_send = 0;
	number_of_bytes_send = sendto(internet_socket, messagebuffer, sizeof(messagebuffer), 0, internet_address, internet_address_length);
	if (number_of_bytes_send == -1)
	{
		perror("sendto");
	}
	printf("debug1: %s\n", messagebuffer);
	int number_of_bytes_received = 0;
	char buffer[1000];
	if (setsockopt(internet_socket, SOL_SOCKET, SO_RCVTIMEO, &timeouts, sizeof(int)) < 0)
	{
	}
	number_of_bytes_received = recvfrom(internet_socket, buffer, (sizeof buffer) - 1, 0, internet_address, &internet_address_length);
	if (number_of_bytes_received == -1)
	{
		perror("recvfrom");
		timeout = 1;
	}
	else
	{
		buffer[number_of_bytes_received] = '\0';
		printf("Received  : %s\n", buffer);
		sprintf(messagebuffer, "%s", buffer);
		trigger1 = 1;
	}
}

void cleanupudp(int internet_socket, struct sockaddr *internet_address)
{
	// Step 3.2
	free(internet_address);

	// Step 3.1
	close(internet_socket);
}
int initializationserver()
{
	//Step 1.1
	struct addrinfo internet_address_setup;
	struct addrinfo * internet_address_result;
	memset( &internet_address_setup, 0, sizeof internet_address_setup );
	//internet_address_setup.ai_family = AF_UNSPEC; //ipv6
    internet_address_setup.ai_family = AF_INET; //ipv4
	internet_address_setup.ai_socktype = SOCK_STREAM; //tcp
	internet_address_setup.ai_flags = AI_PASSIVE; //server
	int getaddrinfo_return = getaddrinfo("192.168.1.92", "24042", &internet_address_setup, &internet_address_result );
	if( getaddrinfo_return != 0 )
	{
		fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( getaddrinfo_return ) );
		exit( 1 );
	}

	int internet_socket = -1;
	struct addrinfo * internet_address_result_iterator = internet_address_result;
	while( internet_address_result_iterator != NULL )
	{
		//Step 1.2
		internet_socket = socket( internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol );
		if( internet_socket == -1 )
		{
			perror( "socket" );
		}
		else
		{
			//Step 1.3
			int bind_return = bind( internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
			if( bind_return == -1 )
			{
				perror( "bind" );
				close( internet_socket );
			}
			else
			{
				//Step 1.4
				int listen_return = listen( internet_socket, 1 );
				if( listen_return == -1 )
				{
					close( internet_socket );
					perror( "listen" );
				}
				else
				{
					break;
				}
			}
		}
		internet_address_result_iterator = internet_address_result_iterator->ai_next;
	}

	freeaddrinfo( internet_address_result );

	if( internet_socket == -1 )
	{
		fprintf( stderr, "socket: no valid socket address found\n" );
		exit( 2 );
	}

	return internet_socket;
}

int connectionserver( int internet_socket )
{
	//Step 2.1
	struct sockaddr_storage client_internet_address;
	socklen_t client_internet_address_length = sizeof client_internet_address;
	int client_socket = accept( internet_socket, (struct sockaddr *) &client_internet_address, &client_internet_address_length );
	if( client_socket == -1 )
	{
		perror( "accept" );
		close( internet_socket );
		exit( 3 );
	}
	return client_socket;
}

void executionserver( int internet_socket )
{
	//Step 3.1
	int number_of_bytes_received = 0;
	char buffer[1000];
	number_of_bytes_received = recv( internet_socket, buffer, ( sizeof buffer ) - 1, 0 );
	if( number_of_bytes_received == -1 )
	{
		perror( "recv" );
	}
	else
	{
		buffer[number_of_bytes_received] = '\0';
		printf( "Received : %s\n", buffer );
	}
}

void cleanupserver( int internet_socket, int client_internet_socket )
{
	//Step 4.2
	int shutdown_return = shutdown( client_internet_socket, SD_RECEIVE );
	if( shutdown_return == -1 )
	{
		perror( "shutdown" );
	}

	//Step 4.1
	close( client_internet_socket );
	close( internet_socket );
}