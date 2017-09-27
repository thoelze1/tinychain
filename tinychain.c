/*
 * @author Tanner Hoelzel
 * @since 09/16/2017
 *
 * Links:
 *      https://medium.freecodecamp.org/from-what-is-blockchain-to-building-a-blockchain-within-an-hour-4e738efc819d
 * 		https://stackoverflow.com/questions/14409466/simple-hash-functions
 * 		http://ecomunsing.com/build-your-own-blockchain
 * 		http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>			// strncpy
#include <unistd.h>			// sleep
#include <sys/socket.h>		// bind
#include <sys/types.h>		// IN_ADDR
#include <arpa/inet.h>		// host <-> network byte order
#include <netdb.h>			// gethostbyname
#include <netinet/tcp.h>	// TCP_NODELAY
#include <errno.h>

#define TRADE_CAP 10

/*
 * A trade has a payer, payee, and amount.
 * 
 * Payer 0 is the mint
 */
typedef struct trade {
	unsigned int payer, payee;
	int amt;
	time_t time;
} Trade;

/*
 * This is exactly the data that is hashed.
 */
typedef struct data {
	unsigned int height, prev_hash, num_trades;
	Trade trades[TRADE_CAP];
	time_t time;
} Data;

/*
 * A block has a parent block, a child block, data,
 * and a hash.
 *
 * These members may have to change to accomodate
 * chain forking.
 */
typedef struct block {
	struct block *prev, *next;
	Data data;
	unsigned int hash;
} Block;


void print_block(Block *block) {
	unsigned int counter;
	printf("Block: %2d\n    Prev Hash: %u\n    Trades: %u\n", block->data.height, block->data.prev_hash, block->data.num_trades);
	for(counter = 0; counter < block->data.num_trades; counter++) {
			printf("        %.4d from %.4u to %.4u\n", block->data.trades[counter].amt, block->data.trades[counter].payer, block->data.trades[counter].payee);
	}
	printf("    Hash: %u\n\n", block->hash);
}

void print_chain(Block *block) {
	Block *curr = block;
	while(curr->next != NULL) {
		print_block(curr);
		curr = curr->next;
	}
}

/*
 * Implementation of djb2
 *
 * Returns hash as unsigned int
 */
unsigned int tiny_hash(Data *data) {
	unsigned int hash_address = 5831, counter;
	for(counter = 0; counter < sizeof(Data); counter++) {
		hash_address = ((hash_address << 5) + hash_address) + ((char *)data)[counter];
	}
	return hash_address;
}

/*
 * Creates first block
 */
Block *create_genesis_block() {
	// Genesis block;
	Block *genesis_block = (Block *)malloc(sizeof(Block));
	genesis_block->prev = NULL;
	genesis_block->next = NULL;
	genesis_block->data.height = 0;
	genesis_block->data.prev_hash = 0;
	genesis_block->data.num_trades = 2;
	genesis_block->data.time = time(NULL);
	genesis_block->data.trades[0] = (Trade) {0,1,100,time(NULL)};
	genesis_block->data.trades[1] = (Trade) {0,2,100,time(NULL)};
	genesis_block->hash = tiny_hash(&(genesis_block->data));
	return genesis_block;
}

/*
 * Creates new block
 */
Block *create_next_block(Block *curr_block) {
	// Next block
	Block *next_block = (Block *)malloc(sizeof(Block));
	next_block->prev = curr_block;
	next_block->next = NULL;
	next_block->data.height = curr_block->data.height + 1;
	next_block->data.prev_hash = curr_block->hash;
	next_block->data.num_trades = curr_block->data.num_trades;;
	next_block->data.time = time(NULL);
	next_block->hash = tiny_hash(&(next_block->data));
	// Prev block
	curr_block->next = next_block;
	return next_block;
}

void build_chain(Block *genesis_block) {
	Block *head = genesis_block;
	for(int counter = 0; counter < 5; counter++) {
		head = create_next_block(head);
	}
}

void broadcast_chain(Block *genesis_block) {

	// Blockchain iterator
	Block *curr;

	// Server socket
	int serversockfd, clientsockfd, n;
	struct sockaddr_in server_addr;

	// "socket descriptor"
	serversockfd = socket(PF_INET, SOCK_STREAM, 0);	// starts at 3, increments

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = PF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(5353);

	// request that server_addr be bound to listen_fd, which is likely 3
	bind(serversockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

	// allow incoming connections, with 10 as queue limit
	// if connection request arrives and queue is full, the connetion is refused
	listen(serversockfd, 10);

	while(1) {
		// To do: implement signal handling to allow graceful exit

		// extracts first connection request in the queue,
		// copies socket with a new file descriptor
		// (blocks until a connection is present)
	printf("Here\n");
		clientsockfd = accept(serversockfd, (struct sockaddr *)NULL, NULL);
	printf("Here\n");

		// write to new node, block by block
		while(curr != NULL) {
			write(clientsockfd, curr, sizeof(Block));
			curr = curr->next;
		}

		close(clientsockfd);
		sleep(1);
	}
}

Block *accept_chain(unsigned int port) {

	// Create a first block to build chain up
	Block *genesis_block, *curr_block;

	// Client socket
	int sockfd, n, optval;
	struct sockaddr_in server_addr;
	char *ip = "127.0.0.1";

	// "socket descriptor"
	sockfd = socket(PF_INET, SOCK_STREAM, 0);	// starts at 3, increments
	if(sockfd < 0) {
		printf("Could not create socket\n");
		exit(1);
	}
	printf("%d\n", sockfd);

	// to override OS interpreting socket reuse
	// between get_node_ports and accept_chain
	// https://gist.github.com/nolim1t/126991
	// https://stackoverflow.com/questions/26297068/c-socket-connection-refused-at-the-second-try
	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	if(sockfd == -1) {
		printf("Error: %s\n", strerror(errno));
		exit(1);
	}

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	// converts address in presentation format to network format
	if(inet_pton(PF_INET, ip, &server_addr.sin_addr) <= 0) {
		printf("inet_pton error\n");
		exit(1);
	}

	if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("Could not call connect: %s\n", strerror(errno));
		exit(1);
	}

	genesis_block = (Block *)malloc(sizeof(Block));
	read(sockfd, genesis_block, sizeof(Block));
	curr_block = (Block *)malloc(sizeof(Block));
	while(read(sockfd, curr_block, sizeof(Block)) > 0);

	shutdown(sockfd, SHUT_RDWR); 
	close(sockfd);

	return genesis_block;
}

unsigned int get_node_ports(unsigned int portserverport, unsigned int port) {
	// Client socket
	int sockfd, n, optval;
	char *recv_buf = (char *)malloc(1024);
	char *send_buf = (char *)malloc(1024);
	struct sockaddr_in server_addr;
	char *ip = "127.0.0.1";

	// "socket descriptor"
	sockfd = socket(PF_INET, SOCK_STREAM, 0);	// starts at 3, increments
	if(sockfd < 0) {
		printf("Could not create socket\n");
		exit(1);
	}
	printf("%d\n", sockfd);
	
	// to override OS interpreting socket reuse
	// between get_node_ports and accept_chain
	// https://gist.github.com/nolim1t/126991
	// https://stackoverflow.com/questions/26297068/c-socket-connection-refused-at-the-second-try
	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	if(sockfd == -1) {
		printf("Error: %s\n", strerror(errno));
		exit(1);
	}


	memset(recv_buf, '\0', sizeof(1024));

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(portserverport);

	// converts address in presentation format to network format
	if(inet_pton(PF_INET, ip, &server_addr.sin_addr) <= 0) {
		printf("inet_pton error\n");
		exit(1);
	}

	if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("Could not call connect\n");
		exit(1);
	}

	n = read(sockfd, recv_buf, 1024-1);

	snprintf(send_buf, 1024, "%d\n", port);
    write(sockfd, send_buf, strlen(send_buf));

	// we know these are working because we can see sockfd
	// being "decremented"
	//
	// I am commenting these due to this link:
	// https://stackoverflow.com/questions/26297068/c-socket-connection-refused-at-the-second-try
	// shutdown(sockfd, SHUT_RDWR); 
	close(sockfd);

	return atoi(recv_buf);
}

int main(int argc, char **argv) {
	srand(time(NULL));
	// A node has its own copy of the blockchain,
	// starting with the genesis block
	Block *genesis_block;
	// We determine if this is the first node
    unsigned int this_node_port, other_node_port;
    this_node_port = 5353 + (rand() % 10);
	other_node_port = get_node_ports(5353, this_node_port);
	if(other_node_port == 0) {
		genesis_block = create_genesis_block();
		build_chain(genesis_block);
		printf("PRINTING INITIAL NODE\n");
		print_chain(genesis_block);
	} else {
		printf("Now accepting on port %4d from port %4d\n", this_node_port, other_node_port);
		genesis_block = accept_chain(other_node_port);
		printf("PRINTING SECONDARY NODE\n");
		print_chain(genesis_block);
	}
	// Now that we have a blockchain, we will broadcast it
	printf("Now broadcasting on port %4d\n", this_node_port);
	broadcast_chain(genesis_block);
	
	/*
	if(fork() == 0) broadcast_chain(genesis_block);
	else {

	}
	*/
	return 0;
}
