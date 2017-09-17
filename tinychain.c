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
#include <string.h> // strncpy
#include <unistd.h> // sleep

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
	sleep(1);
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

int main() {
	srand(time(NULL));
	// Create chain
	Block *head, *genesis_block;
	genesis_block = create_genesis_block();
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
	return 0;
}
