// ECE322 Systems Programming: Project 1
// Parent process of matrix multiplication using pipes
// parent_mat_mult.c
// Grant Kimes

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>


struct childPipes
{
	int childNum;
	pid_t pidNum;
	int parentSend[2];
	int childSend[2];
};

void print_pipeList(int numPipes, struct childPipes pipeList[])
{
	int i;
	for (i = 0; i < numPipes; i++)
		printf("Child %d: parentSend{%d, %d}, childSend{%d, %d}\n", i, pipeList[i].parentSend[0], pipeList[i].parentSend[1], pipeList[i].childSend[0], pipeList[i].childSend[1]);
}

void read_to_matrix(int height, int width, float ** matrix, char * filename)
{
	// Read from filename into 2D float matrix by row major form. Non-equal row and column numbers allowed.

	float num;
	int x, y;
	FILE * fin = fopen(filename, "r");

	if (fin != NULL)
	{
		for (y = 0; y < height; y++)
			for (x = 0; x < width; x++)
			{
				fscanf(fin, "%f", &num);
				matrix[y][x] = num;
			}
	}
	else
	{
		perror("In read_to_matrix");
		exit(1);
	}
	fclose(fin);
}

void close_pipes_except(int childNum, struct childPipes pipeList[], int numChildren)
{
	int i;
	for (i = 0; i < numChildren; i++)
	{
		if (i != childNum)						// Close all pipes that other children are using. 
		{
			close(pipeList[i].parentSend[0]);
			close(pipeList[i].parentSend[1]);
			close(pipeList[i].childSend[0]);
			close(pipeList[i].childSend[1]);
		}
		else if (i == childNum)					// For current child, close ends that are not needed.
		{
			close(pipeList[i].parentSend[1]);	// Close write end of parent's writing pipe.
			close(pipeList[i].childSend[0]);	// Close read end of child's writing pipe. 
		}
	}
}

void child_process_actions(int childNum, struct childPipes pipeList[], int numChildren)
{
	// Called from a child process.

	/* Close all pipes except current child's read from parent and write to parent. */
    close_pipes_except(childNum, pipeList, numChildren);		

	/* Dup current read and write pipe in pipeList to STDIN and STDOUT for this process. */
    dup2(pipeList[childNum].parentSend[0], STDIN_FILENO);
    dup2(pipeList[childNum].childSend[1], STDOUT_FILENO);

	char n[4];
	sprintf(n, "%d", childNum);			// Give childNum as argument to executed process.

	/* Run multiplication program. Will wait for input as STDIN, which will come from parent writing data. */
	/* Result will be written to STDOUT, which will be read from parent process through pipe. */
    int x = execl("ChildMult", "ChildMult", n, 0);
	if (x < 0)
        perror("Error executing ChildMult\n");

    exit(1);	
}

void spawn_child_processes(int numChildren, struct childPipes pipeList[])
{
	/* Called from parent process, creates specified number of children and their respective actions are executed. */

	pid_t pid, curr_pid;
	int childNum, i;

    for (i = 0; i < numChildren; i++)
    {
        pid = fork();               // pid set to 0 if in child process, PID of child if in parent process.
        curr_pid = getpid();        // PID of current process after fork.
        childNum = i;

        if (pid == 0)   			// Child process, execute other child process actions.
            child_process_actions(childNum, pipeList, numChildren);

        pipeList[i].pidNum = pid;

		close(pipeList[i].parentSend[0]);	// Parent close the read end of its sending pipe.
		close(pipeList[i].childSend[1]);	// Parent close the write end of its child sending pipe.
    }
}

void send_rows(int height, int width, int startingRow, int endingRow, int childNum, float ** matrix, FILE * F)
{
	/* Takes a matrix and given pipe file to send to. Sends specified number of rows. */

	fprintf(F, "%d\n", endingRow - startingRow);	// Height of matrix being sent.
	fprintf(F, "%d\n", width);						// Width of matrix being sent.

	int x, y;
	for (y = startingRow; y < endingRow; y++)		// Loop through given portion of matrix being sent, writing each value.
		for (x = 0; x < width; x++)
			fprintf(F, "%f\n", matrix[y][x]);
}

void output_data_to_pipes(int m, int n, int p, int numChildren, struct childPipes pipeList[], float ** matrixA, float ** matrixB)
{
	int i;
	for (i = 0; i < numChildren; i++)
	{
	    int startingRow = i * m / numChildren;     								// Send a portion of matrixA's rows. Based on how many child processes there are.
	    int endingRow = ((i+1) * m / numChildren);

		FILE * parentSendFILE = fdopen(pipeList[i].parentSend[1], "w");			// Open current process's parentSend for writing.
		if (parentSendFILE == NULL)
			perror("In output_data_to_pipes");

		send_rows(m, n, startingRow, endingRow, i, matrixA, parentSendFILE);	// Send portion of rows from matrixA to multiplying process.
		send_rows(n, p, 0, n, i, matrixB, parentSendFILE);						// Send whole matrixB to multiplying process.

		fclose(parentSendFILE);
	}
}

void read_results_from_pipes(int m, int n, int p, int numChildren, struct childPipes pipeList[])
{
	/* Open a file to write results to. Results recieved from pipes from children mult processes. */

	char filename[64];
	sprintf(filename, "result_%dx%d_%dchildren.dat", m, p, numChildren);
	FILE * FOUT = fopen(filename, "w");
	if (FOUT == NULL)
		perror("In read_results_from_pipes");
	
	int i;
	for (i = 0; i < numChildren; i++)
	{
		FILE * FIN = fdopen(pipeList[i].childSend[0], "r");			// Read from current child process's result output.
		if (FIN == NULL)
			perror("In read_results_from_pipes");

		int height, width, childNum;
		fscanf(FIN, "%d", &childNum);
		fscanf(FIN, "%d", &height);
		fscanf(FIN, "%d", &width);

		int y, x;
		float num;
		for (y = 0; y < height; y++)
			for (x = 0; x < width; x++)
			{
				fscanf(FIN, "%f", &num);							// Read from child pipe.
				fprintf(FOUT, "%f\n", num);							// Write to output file.
			}
		fclose(FIN);
	}

	fclose(FOUT);
	printf("Wrote result to '%s'\n", filename);
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

	/* matrixA is (m x n), allocate space as an array of pointers. */
	float ** matrixA = (float **) malloc(m * sizeof(float *) );
    for (i = 0; i < m; i++)
        matrixA[i] = (float *) malloc(n * sizeof(float) );

	/* matrixB is (n x p), allocate space as an array of pointers. */
	float ** matrixB = (float **) malloc(n * sizeof(float *) );
    for (i = 0; i < n; i++)
        matrixB[i] = (float *) malloc(p * sizeof(float) );

	/* Read data from files into matrixA (m x n) and matrixB (n x p). */
	read_to_matrix(m, n, matrixA, "mat100x100.dat");
	read_to_matrix(n, p, matrixB, "mat100x100_1.dat");

	
	/* Initialize all pipes in array of childPipes structs. */
	struct childPipes pipeList[c];
	for (i = 0; i < c; i++)
	{
		pipe( pipeList[i].parentSend );
		pipe( pipeList[i].childSend );
		pipeList[i].childNum = i;
	}

	/* Make child processes. Child processes exec a new file as they are created. Only parent process returns here after spawn_child_processes(). */
	spawn_child_processes(c, pipeList);

	/* Write data to corresponding pipes for processes to compute matrix multiplication. */
	output_data_to_pipes(m, n, p, c, pipeList, matrixA, matrixB);

	while(wait() > 0) {;}

	/* Read from pipes and output results to a file. */
	read_results_from_pipes(m, n, p, c, pipeList);

	exit(0);
}
