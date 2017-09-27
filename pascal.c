#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

	int row_n, num_vals, val_i, row, row_i, par1, par2, *pascal;
		
	row_n = atoi(argv[1]);

	// dynamically allocate pascal to (n+1) choose 2
	num_vals = 55;
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

		
