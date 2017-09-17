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

/*
 * A block has a height, hash,
 * previous_hash, an amount of data, and a time.
 *
 * These members may have to change to accomodate
 * chain forking.
 */
typedef struct block {
	struct block *next, *prev;
	unsigned int height, hash, prev_hash, data_size;
	void *data;
	time_t time;
} Block;


void print_block(Block *block) {
	printf("Block %2d\n\tHash: %u\n\tTime: %lu\n", block->height, block->hash, block->time);
}

/*
 * Implementation of djb2
 *
 * Returns hash as unsigned int
 */
unsigned int tiny_hash(void *data, unsigned int data_size) {
	unsigned int hash_address = 5831, counter;
	for(counter = 0; counter < data_size; counter++) {
		hash_address = ((hash_address << 5) + hash_address) + ((char *)data)[counter];
	}
	return hash_address;
}

/*
 * Creates first block
 */
Block *create_genesis_block() {
	Block *genesis_block = (Block *) malloc (sizeof(Block));
	genesis_block->prev = NULL;
	genesis_block->next = NULL;
	genesis_block->height = 0;
	genesis_block->data = malloc(13);
	genesis_block->data_size = 13;
	strncpy(genesis_block->data, "Genesis block", 13);
	genesis_block->prev_hash = 0;
	genesis_block->hash = tiny_hash(genesis_block->data, genesis_block->data_size);
    genesis_block->time = time(NULL);
	return genesis_block;
}

/*
 * Creates new block
 */
Block *create_next_block(Block *curr_block) {
	Block *next_block = (Block *) malloc (sizeof(Block));
	curr_block->next = next_block;
	next_block->prev = curr_block;
	next_block->next = NULL;
	next_block->height = curr_block->height + 1;
	next_block->data = malloc(sizeof(unsigned int));
	*((unsigned int *)(next_block->data)) = (unsigned int)rand();
	next_block->data_size = sizeof(unsigned int);
	next_block->prev_hash = curr_block->hash;
	next_block->hash = tiny_hash(next_block->data, next_block->data_size);
    next_block->time = time(NULL);
	return next_block;
}

void build_chain(Block *genesis_block) {
	Block *head;
	// Create chain
	head = genesis_block;
	for(int counter = 0; counter < 5; counter++) {
		head = create_next_block(head);
	}
	// Print chain
	head = genesis_block;
	while(head->next != NULL) {
		print_block(head);
		head = head->next;
	}
}

int main(int argc, char **argv) {
	srand(time(NULL));
	Block *genesis_block = create_genesis_block();
	
	// Server socket
	int sockfd, connfd, n;
	char send_buf[1024], recv_buf[1024];
	struct sockaddr_in server_addr, dest_addr;
	time_t ticks;

	// "socket descriptor"
	sockfd = socket(PF_INET, SOCK_STREAM, 0);	// starts at 3, increments
	connfd = 0;

	memset(send_buf, '0', sizeof(send_buf));
	memset(recv_buf, '0', sizeof(recv_buf));

	memset(&server_addr, '0', sizeof(server_addr));
	server_addr.sin_family = PF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(5000);

	// request that server_addr be bound to listen_fd, which is likely 3
	bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

	// allow incoming connections, with 10 as queue limit
	// if connection request arrives and queue is full, the connetion is refused
	listen(sockfd, 10);

	while(1) {
		// extracts first connection request in the queue,
		// copies socket with a new file descriptor
		// (blocks until a connection is present)

		connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);

		ticks = time(NULL);
		snprintf(send_buf, sizeof(send_buf), "%.24s\r\n", ctime(&ticks));
		write(connfd, send_buf, strlen(send_buf));

		close(connfd);
		sleep(1);
	}

    if(argc != 2) {
		printf("usage: %s <server ip>\n", argv[0]);
		exit(1);
	}

	// converts address in presentation format to network format 
	inet_pton(PF_INET, argv[1], &serv_addr.sin_addr);

	connect(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	while((n = read(sockfd, recv_buf, sizeof(recv_buf)-1)) > 0) {
		recv_buf[n] = 0;
		fputs(recv_buf, stdout);
	}

	return 0;
}
