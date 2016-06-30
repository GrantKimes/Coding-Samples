// ECE322 Systems Programming Project 2
// Server for matrix Multiplication using networking
// server.c - Listen on a port until the client sends data to multiply. Send back multiplication results to client. 
// Compiled to 'Server'.
// Grant Kimes

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/errno.h>
extern int errno;

void read_matrix_from_socket_file(FILE * F, int height, int width, float ** matrix)
{
	// Read data from client into server's matrix.

	int y, x;
	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
			fscanf(F, "%f", & matrix[y][x]);
}

void multiply_and_send(FILE * F, int m, int n, int p, float ** matrixA, float ** matrixB)
{
	// Output results of multiplication to client.

	int i;
	float sum;
	int y, x;

	// Output dimensions of result matrix: m x p. 
	fprintf(F, "%d\n", m);
	fprintf(F, "%d\n", p);

	// Output[y][x] = row y of matrixA * column x of matrixB. 
	for (y = 0; y < m; y++)
	{
		for (x = 0; x < p; x++)
		{
			sum = 0;
			for (i = 0; i < n; i++)    
				sum += matrixA[y][i] * matrixB[i][x];
			fprintf(F, "%f\n", sum);
		}
	}
	fclose(F);
}

int main(int argc, char * argv[])
{
	// matrixA is (m x n)
	// matrixB is (n x p)
	// Result is (m x p)
	// Input from client is: m, n, data(matrixA), n, p, data(matrixB)
	// Output to client is: m, p, data(result)

	if (argc != 2 || atoi(argv[1])<1)
	{
		printf("Incorrect number of arguments, usage: Server <portNum> \n");
		exit(1);
	}
	int portNum = atoi(argv[1]);

	int m;		// matrixA height.
	int n;		// matrixA width, matrixB height.
	int p;		// matrixB width.
	float ** matrixA;
	float ** matrixB;
	int i;		// For loop counter.

	struct sockaddr_in sin;		// For socket information.
	struct sockaddr_in fsin;	// For client socket address. 
	unsigned int fsinSize = sizeof(fsin);
	int socket_descriptor;		// Socket descriptor.
	int ssock;					// Communication socket.

	// Fill info for socket information struct. getaddrinfo() alternative.
	memset(&sin, 0, sizeof(sin));		// Initialize sin struct to 0.
	sin.sin_family = AF_INET;			// Set address family.
	sin.sin_addr.s_addr = INADDR_ANY;	// Set the local server address.
	sin.sin_port = htons((unsigned short) portNum);		// Set the local server port. Preferrably > 40000.

	// Get a socket, bind it to a port, and listen for connections to that port. 
	socket_descriptor = socket(PF_INET, SOCK_STREAM, 0);	// Create the socket.
	if (socket_descriptor < 0)
	{
		printf("Server cannot get socket. Error: %s\n", strerror(errno));
		exit(1);
	}
	if (bind(socket_descriptor, (struct sockaddr *) &sin, sizeof(sin)) < 0)		// Bind the socket to the local address and port.
	{
		printf("Server cannot bind socket. Error: %s\n", strerror(errno));
		exit(1);
	}
	if (listen(socket_descriptor, 10) < 0)
	{
		printf("Server cannot listen. Error: %s\n", strerror(errno));
		exit(1);
	}


	printf("Server listening on port %d...\n", portNum);

	ssock = accept(socket_descriptor, (struct sockaddr *) &fsin, &fsinSize);	// Wait for incoming client connections.
	if (ssock < 0)
	{
		printf("Server problem accepting socket. Error: %s\n", strerror(errno));
		exit(1);
	}

	FILE * F = fdopen(ssock, "w+");		// Open file using given socket descriptor from accept. 
	if (F == NULL)
	{
		perror("Server error opening file");
		exit(1);
	}

	// Read in first matrix from client.
	fscanf(F, "%d", &m);
	fscanf(F, "%d", &n);
	matrixA = (float **) malloc(m * sizeof(float *));
	for (i = 0; i < m; i++)
		matrixA[i] = (float *) malloc(n * sizeof(float));
	read_matrix_from_socket_file(F, m, n, matrixA);

	// Read in second matrix from client.
	fscanf(F, "%d", &n);
	fscanf(F, "%d", &p);
	matrixB = (float **) malloc(n * sizeof(float *));
	for (i = 0; i < n; i++)
		matrixB[i] = (float *) malloc(p * sizeof(float));
	read_matrix_from_socket_file(F, n, p, matrixB);

	// Multiply inputted matrices, and send result back to client.
	multiply_and_send(F, m, n, p, matrixA, matrixB);


	close(ssock);
	close(socket_descriptor);

	exit(0);
}
