Client-Server Socket Networking (C)
==================================
The client program reads input data from matrix files. It determines the dimensions of multiplication to perform, and how many child processes to delegate to.
The client then splits up the data, creates a new process for each portion, and the new processes communicate with waiting server processes that recieve the data.
Each server process calculates multiplication on the given portion, and responds with the result. 
The client recieves these pieces from each process and puts them back together for a final result.
