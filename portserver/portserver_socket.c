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

void server_socket() {
	// Server socket
	int sockfd, connfd, n;
	char send_buf[1025];
	struct sockaddr_in server_addr;
	time_t ticks;

	// "socket descriptor"
	sockfd = socket(PF_INET, SOCK_STREAM, 0);	// starts at 3, increments

	memset(send_buf, '\0', sizeof(send_buf));

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = PF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(5353);

	// request that server_addr be bound to listen_fd, which is likely 3
	bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

	// allow incoming connections, with 10 as queue limit
	// if connection request arrives and queue is full, the connetion is refused
	listen(sockfd, 10);

	// extracts first connection request in the queue,
	// copies socket with a new file descriptor
	// (blocks until a connection is present)

	connfd = accept(sockfd, (struct sockaddr *)NULL, NULL);

	ticks = time(NULL);
	snprintf(send_buf, sizeof(send_buf), "127.0.0.1:6363\n");
	write(connfd, send_buf, strlen(send_buf));

	shutdown(connfd, SHUT_RDWR);
	close(connfd);
	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);

}

int main(int argc, char **argv) {
	server_socket();
	return 0;
}
