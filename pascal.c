/**
 * @author Tanner Hoelzel
 * @since 09/27/2017
 *
 * Prints first n rows of Pascal's triangle in θ(n choose 2).
 *
 * Computing number of entries is θ(n).
 * Computing the n choose 2 entries is θ(n choose 2).
 * 
 * Next I'm going to codegolf this in python.
 */

#include <stdio.h>
#include <stdlib.h>

int rows_to_entries(int rows) {

	int num_ents, curr_row;

	for(num_ents = 0, curr_row = rows; curr_row > 0; curr_row--) {
		num_ents += curr_row;
	}

	return num_ents;

}

int main(int argc, char **argv) {

	int row_n, num_vals, val_i, row, row_i, par1, par2, *pascal;
		
	row_n = atoi(argv[1]);

	// dynamically allocate pascal to (n+1) choose 2
	num_vals = rows_to_entries(row_n);
	pascal = (int *)malloc(sizeof(int)*num_vals);

	// fill and print row 1	
	pascal[0] = 1;
	printf("%3d\n", 1);

	// fills and print rows 2 to n
	row = 2;
	for(val_i = 1, row_i = 0; val_i < num_vals; val_i++, row_i++) {
		// Determine positon in row
		if(row_i == row) {
			printf("\n");
			row++;
			row_i = 0;
		}
		// Determine parents
		par1 = (row_i == 0)? 0 : pascal[val_i - row];
		par2 = (row_i == row - 1)? 0: pascal[val_i - row + 1];
		// Compute value	
		pascal[val_i] = par1 + par2;
		// Print value
		printf("%3d ", pascal[val_i]);
	}

	printf("\n");

	return 0;

}

		
