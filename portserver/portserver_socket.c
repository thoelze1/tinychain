/*
 * @author Tanner Hoelzel
 * @since 09/16/2017
 *
 * Links:
 *   http://www.thegeekstuff.com/2011/12/c-socket-programming/?utm_source=feedburner
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>			// strncpy
#include <unistd.h>			// sleep
#include <sys/socket.h>		// bind
#include <sys/types.h>		// IN_ADDR
#include <arpa/inet.h>		// host <-> network byte order

#define MAX 10

void server_socket() {

	// Keep track of ports of full nodes
	int node_ports[MAX];
	unsigned int num_ports;

	// Server socket
	int sockfd, newsockfd, n;
	char send_buf[1024], recv_buf[1024];
	struct sockaddr_in server_addr;

	// "socket descriptor"
	sockfd = socket(PF_INET, SOCK_STREAM, 0);	// starts at 3, increments

	memset(node_ports, 0, sizeof(node_ports));
	memset(send_buf, '\0', sizeof(send_buf));
	memset(recv_buf, '\0', sizeof(recv_buf));

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = PF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(5353);

	// request that server_addr be bound to listen_fd, which is likely 3
	bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

	// allow incoming connections, with 10 as queue limit
	// if connection request arrives and queue is full, the connetion is refused
	listen(sockfd, 10);

	num_ports = 0;
	while(1) {
		// To do: implement signal handling to allow graceful exit

		// extracts first connection request in the queue,
		// copies socket with a new file descriptor
		// (blocks until a connection is present)
		newsockfd = accept(sockfd, (struct sockaddr *)NULL, NULL);

		snprintf(send_buf, sizeof(send_buf), "%d\n", node_ports[0]);
		write(newsockfd, send_buf, strlen(send_buf));

		/*
		while((n = read(clientsockfd, recv_buf, 1024-1)) > 0) {
			recv_buf[n] = 0;
			if(fputs(recv_buf, stdout) == EOF) printf("fputs error\n");
		}
		*/
		n = read(clientsockfd, recv_buf, 1024-1);

		printf("New port: %d\n", atoi(recv_buf));
		node_ports[num_ports%MAX] = atoi(recv_buf);
		num_ports++;

		close(newsockfd);
	}
}

int main(int argc, char **argv) {
	server_socket();
	return 0;
}
