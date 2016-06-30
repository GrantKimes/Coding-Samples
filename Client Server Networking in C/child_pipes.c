// ECE322 Systems Programming Project 1
// Matrix Multiplication using Pipes
// child_mult.c
// Grant Kimes

#include <stdio.h>

void read_matrix(int height, int width, float ** matrix)
{
	int y, x;
	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
		{
			scanf("%f", & matrix[y][x]);
			//printf("  Recieved %f\n", matrix[y][x]);
		}

	//printf("   Recieved everything for matrix\n");
}

void multiply_matrices(int m, int n, int p, float ** matrixA, float ** matrixB)
{
	int i;
	float sum;
	int y, x;

	/* Output dimensions of result matrix: m x p. */
	printf("%d\n", m);
	printf("%d\n", p);

	/* Output[y][x] = row y of matrixA * column x of matrixB. */
	for (y = 0; y < m; y++)
	{
		for (x = 0; x < p; x++)
		{
			sum = 0;
			for (i = 0; i < n; i++)    
				sum += matrixA[y][i] * matrixB[i][x];
			printf("%f\n", sum);
		}
	}

}

int main(int argc, char * argv[])
{
	// matrixA is (m x n)
	// matrixB is (n x p)
	// Result is (m x p)
	// Input from STDIN is: m, n, data(matrixA), n, p, data(matrixB)
	// Output to STDOUT is m, p, data(result)

	if (argc != 2)
	{
		printf("Incorrect number of arguments (%d), expected 2.\n", argc);
		exit(1);
	}
	int childNum = atoi(argv[1]);

	int m;	// matrixA height.
	int n;	// matrixA width, matrixB height.
	int p;	// matrixB width.
	int i;	// For loop counter.

	/* Read in m, n. Allocate matrixA with those dimensions. Read in data to matrixA. */
	scanf("%d", &m);
	scanf("%d", &n);
	float ** matrixA = (float **) malloc(m * sizeof(float *) );		// Array of pointers to each row of float values.
	for (i = 0; i < m; i++)
		matrixA[i] = (float *) malloc(n * sizeof(float) );
	read_matrix(m, n, matrixA);

	/* Read in n, p. Allocate matrixB with those dimensions. Read in data to matrixB. */
	scanf("%d", &n);
	scanf("%d", &p);
	float ** matrixB = (float **) malloc(n * sizeof(float *) );
	for (i = 0; i < n; i++)
		matrixB[i] = (float *) malloc(p * sizeof(float) );
	read_matrix(n, p, matrixB);

	/* Output to STDOUT childNum of current process. */
	printf("%d\n", childNum);

	/* Multiply matrixA and matrixB. Result matrix is outputted to STDOUT. */
	multiply_matrices(m, n, p, matrixA, matrixB);

	return 0;
}
