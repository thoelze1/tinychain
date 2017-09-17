/*
 * @author Tanner Hoelzel
 * @since 09/16/2017
 *
 * Links:
 * 		https://stackoverflow.com/questions/14409466/simple-hash-functions
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>			// strncpy
#include <unistd.h>			// sleep
#include <sys/socket.h>		// bind
#include <sys/types.h>		// IN_ADDR
#include <arpa/inet.h>		// host <-> network byte order

void client_socket(char *ip) {
	// Client socket
	int sockfd, n;
	char recv_buf[1024];
	struct sockaddr_in server_addr;
	time_t ticks;

	// "socket descriptor"
	sockfd = socket(PF_INET, SOCK_STREAM, 0);	// starts at 3, increments
	if(sockfd < 0) {
		printf("Could not create socket\n");
		exit(1);
	}

	memset(recv_buf, '\0', sizeof(recv_buf));

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(5000);

	// converts address in presentation format to network format 
	if(inet_pton(PF_INET, ip, &server_addr.sin_addr) <= 0) {
		printf("inet_pton error\n");
		exit(1);
	}

	printf("2\n");
	if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("Could not call connect\n");
		exit(1);
	}

	while((n = read(sockfd, recv_buf, sizeof(recv_buf)-1)) > 0) {
		recv_buf[n] = 0;
		if(fputs(recv_buf, stdout) == EOF) printf("fputs error\n");
		printf("Hi\n");
	}
}

int main(int argc, char **argv) {
    if(argc != 2) {
		printf("usage: ./prog <server ip>\n");
		exit(1);
	}
	client_socket(argv[1]);
	return 0;
}
