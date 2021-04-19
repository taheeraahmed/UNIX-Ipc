#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

// Exercise a)
// Create pipe - split into two processes
    // -> child process writes message og given size continuously
    // -> parent process reads mesasge child has written
    // -> after parent has read -> collect cumulative size of read blocks

// pipefd - array
char* p;

int main(int argc, char *argv[])
{  
    // 1) initialiser, 2) fork, 3) while loop på read/write - skal være så rask som mulig
    // Initialisere alle verdier
    int     tot_bytes = 0; 
    pid_t   cpid;
    long    n_bytes = strtol(argv[1], &p, 10);
    int     pipefd[2];
    char    block_size[n_bytes];

    // Takes input argv ikke scanf

    // Create pipe
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    // Split into two processes
    if ((cpid = fork()) == -1) 
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    while(1)
    {
        if (cpid == 0)
        {
            // Closing this end of the pipe because the child process doesn't need it 
            close(pipefd[0]);
            write(pipefd[1], block_size, n_bytes);
            wait(NULL);         // Remove zombies
            
        }
        // In the parent process (READ)
        else 
        {
            //Parent process doesn't need this end of the pipe
            close(pipefd[1]);
            // Read returnerer antall bytes lest
            n_bytes = read(pipefd[0], argv[1], n_bytes);
            // To get total amount of bytes read
            tot_bytes += n_bytes;
            printf("tot_bytes: %d\n", tot_bytes);
        }
        
    }
    return(0);
}

