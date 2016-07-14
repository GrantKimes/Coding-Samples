// ECE322 Systems Programming: Project 2
// Client of matrix multiplication using networking
// client.c - Reads from matrix files and sends data to servers for computation. Then recieves results back and writes to file.
// Compiled to 'Client'. Run as 'Client <m> <n> <p> <c>'.
// Grant Kimes

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

#include <sys/errno.h>
extern int errno;

struct serverInfo
{
	// Info for each child process acting as a server. 

	int socketDescriptor;
	int portNum;
	FILE * F;
	int childNum;
	pid_t PID;
};

void read_to_matrix(int height, int width, float ** matrix, char * filename)
{
	// Read from filename into 2D float matrix by row major form. Non-equal row and column numbers allowed.

	float num;
	int x, y;
	FILE * fin = fopen(filename, "r");
	if (fin == NULL)
	{
		perror("In read_to_matrix");
		exit(1);
	}

	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
		{
			fscanf(fin, "%f", &num);
			matrix[y][x] = num;
		}

	fclose(fin);
}

void send_rows(FILE * F, int height, int width, int startingRow, int endingRow, int childNum, float ** matrix)
{
	// Takes a matrix and given pipe file to send to. Sends specified number of rows. 

	fprintf(F, "%d\n", endingRow - startingRow);	// Height of matrix being sent.
	fprintf(F, "%d\n", width);						// Width of matrix being sent.

	int x, y;
	for (y = startingRow; y < endingRow; y++)		// Loop through given portion of matrix being sent, writing each value.
		for (x = 0; x < width; x++)
			fprintf(F, "%f\n", matrix[y][x]);

	fflush(F);
}

void send_data_to_servers(struct serverInfo Servers[], int m, int n, int p, int numChildren, float ** matrixA, float ** matrixB)
{
	int i;
	for (i = 0; i < numChildren; i++)
	{
		int startingRow = i * m / numChildren;			// Send corresponding portion of matrixA's rows.
		int endingRow = (i+1) * m / numChildren;

		send_rows(Servers[i].F, m, n, startingRow, endingRow, i, matrixA);
		send_rows(Servers[i].F, n, p, 0, n, i, matrixB);
	}
}

void read_data_from_servers(struct serverInfo Servers[], int m, int n, int p, int numChildren)
{
	char filename[64];
	sprintf(filename, "result%dx%d_%dchildren.txt", m, p, numChildren);
	FILE * FOUT = fopen(filename, "w");

	int i;
	for (i = 0; i < numChildren; i++)
	{
		FILE * currFile = Servers[i].F;
		int height, width, childNum;
		fscanf(currFile, "%d", &height);
		fscanf(currFile, "%d", &width);

		int y, x;
		float num;
		for (y = 0; y < height; y++)
			for (x = 0; x < width; x++)
			{
				fscanf(currFile, "%f", &num);
				fprintf(FOUT, "%f\n", num);
			}

		fclose(currFile);
		close(Servers[i].socketDescriptor);
	}
	fclose(FOUT);
	printf("Client wrote results to %s\n", filename);
}

void server_actions(int portNum)
{
	char n[4];
	sprintf(n, "%d", portNum);			// Give portNum as argument to executed process.

	int x = execl("Server", "Server", n, 0);
	if (x < 0)
		perror("In server_actions");
	exit(1);
}

void start_servers(struct serverInfo Servers[], int numChildren)
{
	char * serverAddress = "localhost";
	int portNum = 40100;				// Arbitrary starting port. Gets incremented for each child.
	int i;
	
	struct hostent *phe;		// For server address.
	struct sockaddr_in sin;		// For socket information.
	int socketDescriptor;		// Socket descriptor.
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;       // IPv4.

	// Check if valid host.
    if (phe = gethostbyname(serverAddress))	
    {
        //printf("Client found host %s\n", phe->h_name);
        memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);  // Copy host name into socket information if valid host.
    }
    else
    {
        printf("Client can't get host. Error: %s\n", strerror(errno));
        exit(1);
    }

	// Spawn given number of children to each run as a server.
	for (i = 0; i < numChildren; i++)
	{
		pid_t PID = fork();
		if (PID == 0)		// Child process to become server.
		{
			//server_actions(portNum);		// Execute server process.
			exit(1);
		}
		else				// Parent process acting as client, connect to each server.
		{
			Servers[i].PID = PID;
			Servers[i].portNum = portNum++;
			Servers[i].childNum = i;
		}
	}

	//sleep(1);
	
	// Initialize connection to each server.
	for (i = 0; i < numChildren; i++)
	{
	    sin.sin_port = htons( (unsigned short) Servers[i].portNum);        // Set socket port number for current server.

	    socketDescriptor = socket(PF_INET, SOCK_STREAM, 0);    // Get new socket number.
	    if (socketDescriptor < 0)
	    {
	        printf("Client cannot get socket. Error: %s\n", strerror(errno));
	        exit(1);
	    }

		// Use socket number to connect to destination in socket info.
	    if ( connect(socketDescriptor, (struct sockaddr *) &sin, sizeof(sin)) < 0) 
	    {
   		    printf("Client cannot connect. Error: %s\n", strerror(errno));
	        exit(1);
		}

	    FILE * F = fdopen(socketDescriptor, "w+");             // Open file to send data to destination through socket.
		if (F == NULL)
		{
			perror("Client error opening file with socketDescriptor");
			exit(1);
		}

		Servers[i].F = F;
		Servers[i].socketDescriptor = socketDescriptor;
	}
}

int main(int argc, char * argv[])
{
	if (argc != 5 || atoi(argv[1])<1 || atoi(argv[2])<1 || atoi(argv[3])<1 || atoi(argv[4])<1 )
	{
		printf("Invalid input, expected: a.out <m> <n> <p> <c>\n");
		printf("matrixA (m x n), matrixB (n x p), # child processes (c)\n");
		exit(1);
	}

	int m = atoi(argv[1]);		// Height of matrixA.
	int n = atoi(argv[2]);		// Width of matrixA, height of matrixB.
	int p = atoi(argv[3]);		// Width of matrixB.
	int c = atoi(argv[4]);		// Number of child processes.
	int i;						// Counter used in all for loops.

	// matrixA is (m x n). Allocate space as an array of pointers, and read from file.
	float ** matrixA = (float **) malloc(m * sizeof(float *) );
    for (i = 0; i < m; i++)
        matrixA[i] = (float *) malloc(n * sizeof(float) );
	read_to_matrix(m, n, matrixA, "mat100x100.dat");

	// matrixB is (n x p). Allocate space as an array of pointers, and read from file.
	float ** matrixB = (float **) malloc(n * sizeof(float *) );
    for (i = 0; i < n; i++)
        matrixB[i] = (float *) malloc(p * sizeof(float) );
	read_to_matrix(n, p, matrixB, "mat100x100_1.dat");

	// Spawn child processes for servers, and connect to each of them.
	struct serverInfo Servers[c];
	start_servers(Servers, c);

	// Output data to each of the servers that are connected to. 
	send_data_to_servers(Servers, m, n, p, c, matrixA, matrixB);

	// Read back the results of the matrix multiplication that the servers did. 
	read_data_from_servers(Servers, m, n, p, c);

	return 0;
}
