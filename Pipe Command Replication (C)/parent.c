/*	Grant Kimes
 *	ECE322 Systems Programming Exam 2
 *	Extra Credit: Combine into one parent (parent.c)
 * 	11/21/15
 *	Parent to simulate command line pipes of programs from part A.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

struct executableFile
{
	//char * filename;
	char * arguments[10];
};

void read_executableFiles(int numExecs, struct executableFile * Execs[], int argc, char * argv[])
{
	/* Read data from command line parameters into array of executableFiles. */

	int currentArgvIndex = 2;									// Argv[1] was numExecs, first exec filename is argv[2].
	int execIndex;
	for (execIndex = 0; execIndex < numExecs; execIndex++)		// Increment through the Execs array, filling in values from argv.
	{
		int j;
		for (j = 0; j < 10; j++)
			Execs[execIndex] = malloc(64 * sizeof(char));		// Allocate 64 chars for each exec argument. 10 args max.

		int i;
		for (i = 0; currentArgvIndex < argc; i++)				// Increment through argv until a 0 is reached.
		{
			if (argv[currentArgvIndex][0] == '0')				// When 0 is reached, 
			{
				Execs[execIndex]->arguments[i] = 0;				// Add NULL terminator in arguments array, 
				currentArgvIndex++;								// Increment next argv,
				break;											// Break to go to next exec.
			}
			Execs[execIndex]->arguments[i] = argv[currentArgvIndex];	// Set current exec's argument to current argv item. 
			currentArgvIndex++;									// Increment to next in argv.
		}
	}

}

void run_executableFiles(int numExecs, struct executableFile * Execs[])
{
	/* Given array of executable files, make a process for each one. Pipe the output from one to the input of the successive one, etc. */

	int i;
	int fdCurr[2];
	int fdPrev[2];

	for (i = 0; i < numExecs; i++)
	{
		fdPrev[0] = fdCurr[0];						// Keep a copy of the previous pipe before making a new one.
		fdPrev[1] = fdCurr[1];

		pipe(fdCurr);								// Make a new pipe.

		/* First exec, don't dup its STDIN. */
		if (i == 0)	
		{
			pid_t PID = fork();
			if (PID == 0)							// Child, exec first program.
			{
				close(fdCurr[0]);					// Close read end of pipe.
				dup2(fdCurr[1], STDOUT_FILENO);		// Dup write to STDOUT.

				int x = execv(Execs[i]->arguments[0], Execs[i]->arguments);		// Pass executable filename as first parameter, then argument vector.
				if (x < 0)
					perror("In first exec");
				exit(1);
			}
		}

		/* Last exec, don't dup its STDOUT. */
		else if (i == numExecs-1)					
		{
			pid_t PID = fork();
			if (PID == 0)							// Child, exec last program.
			{
				close(fdCurr[0]);					// Close curr pipe, only need prev.
				close(fdCurr[1]);

				close(fdPrev[1]);					// Close write end of prev pipe.
				dup2(fdPrev[0], STDIN_FILENO);		// Dup read end of prev pipe to STDIN.

				int x = execv(Execs[i]->arguments[0], Execs[i]->arguments);
				if (x < 0)
					perror("In last exec");
				exit(1);
			}

			close(fdPrev[0]);						// Parent close prev pipe.
			close(fdPrev[1]);
			close(fdCurr[0]);						// Last exec, parent close curr pipe.
			close(fdCurr[1]);
		}

		/* Not first or last exec. Dup prevPipe's output to STDIN, STDOUT to currPipe's output. */
		else										
		{
			pid_t PID = fork();
			if (PID == 0)							// Child, exec program with duped STDIN and STDOUT.
			{
				close(fdPrev[1]);					// Close write of prev pipe.
				dup2(fdPrev[0], STDIN_FILENO);		// Dup prev pipe's read to STDIN.

				close(fdCurr[0]);					// Close read of curr pipe.
				dup2(fdCurr[1], STDOUT_FILENO);		// Dup STDOUT to curr pipe's write.

				int x = execv(Execs[i]->arguments[0], Execs[i]->arguments);		// Pass executable filename as first parameter, then argument vector.
				if (x < 0)
					perror("In middle exec");
				exit(1);
			}

			close(fdPrev[0]);						// Middle exec, parent close prev pipe.
			close(fdPrev[1]);
		}
	}
}


int main(int argc, char * argv[])
{
	/* Input is in format: Parent <# of execs> <execFile1> <arguments...> 0 <execFile2> <arguments...> 0 ... */

	if (argc < 2)
	{
		printf("Invalid number of arguments.\n");
		return 1;
	}

	int numExecs = atoi(argv[1]);
	if (numExecs == 0)
		return 1;

	/* Create an array of executable files, size was an input parameter. */
	struct executableFile * Execs[numExecs];		

	/* Read parameters from argv to array of Execs structs. */
	read_executableFiles(numExecs, Execs, argc, argv);

	/* Cycle through Exec array, making new processes and duping pipes accordingly. */
	run_executableFiles(numExecs, Execs);

	while (wait() > 0) {;}

	return 0;
}
