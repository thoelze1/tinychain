/*
 * @author Tanner Hoelzel
 * @since 09/16/2017
 *
 * Links:
 *      https://medium.freecodecamp.org/from-what-is-blockchain-to-building-a-blockchain-within-an-hour-4e738efc819d
 * 		https://stackoverflow.com/questions/14409466/simple-hash-functions
 * 		http://ecomunsing.com/build-your-own-blockchain
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
	Data *data;
	unsigned int hash;
} Block;


void print_block(Block *block) {
	unsigned int counter;
	printf("Block: %2d\n    Prev Hash: %u\n    Trades: %u\n", block->data->height, block->data->prev_hash, block->data->num_trades);
	for(counter = 0; counter < block->data->num_trades; counter++) {
			printf("        %.4d from %.4u to %.4u\n", block->data->trades[counter].amt, block->data->trades[counter].payer, block->data->trades[counter].payee);
	}
	printf("    Hash: %u\n\n", block->hash);
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
	// Genesis data
	Data *genesis_data = (Data *)malloc(sizeof(Data));
	genesis_data->height = 0;
	genesis_data->prev_hash = 0;
	genesis_data->num_trades = 2;
	genesis_data->trades[0] = (Trade) {0,1,100,time(NULL)};
	genesis_data->trades[1] = (Trade) {0,2,100,time(NULL)};
	genesis_data->time = time(NULL);
	// Genesis block;
	Block *genesis_block = (Block *)malloc(sizeof(Block));
	genesis_block->prev = NULL;
	genesis_block->next = NULL;
	genesis_block->data = genesis_data;
	genesis_block->hash = tiny_hash(genesis_block->data);
	return genesis_block;
}

/*
 * Creates new block
 */
Block *create_next_block(Block *curr_block) {
    // Next data
	Data *next_data = (Data *)malloc(sizeof(Data));
	next_data->height = curr_block->data->height + 1;
	next_data->prev_hash = curr_block->hash;
	next_data->num_trades = 2; // this will change
	next_data->time = time(NULL);
	// Next block
	Block *next_block = (Block *)malloc(sizeof(Block));
	next_block->prev = curr_block;
	next_block->next = NULL;
	next_block->data = next_data;
	next_block->hash = tiny_hash(next_block->data);
	// Prev block
	curr_block->next = next_block;
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

char *get_node_ips(char *ip, unsigned int port) {
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
	server_addr.sin_port = htons(port);

	// converts address in presentation format to network format
	if(inet_pton(PF_INET, ip, &server_addr.sin_addr) <= 0) {
		printf("inet_pton error\n");
		exit(1);
	}

	if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("Could not call connect\n");
		exit(1);
	}

	while((n = read(sockfd, recv_buf, sizeof(recv_buf)-1)) > 0) {
		recv_buf[n] = 0;
		if(fputs(recv_buf, stdout) == EOF) printf("fputs error\n");
	}

	shutdown(sockfd, SHUT_RDWR); 
	close(sockfd); 
}

int main(int argc, char **argv) {
	srand(time(NULL));
	Block *genesis_block = create_genesis_block();
	build_chain(genesis_block);
	char *ip = get_node_ips("127.0.0.1");
	return 0;
}
