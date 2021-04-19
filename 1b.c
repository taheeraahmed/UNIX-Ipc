#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

int     n_bytes; 
long    bandwidth;
int     seconds;
char*   p;
size_t  tot_bytes_sec = 0; 


void sig_handler(int signum)
{
    printf("Bandwidth: %ld bytes/sec\n", tot_bytes_sec);
    alarm(1);
    tot_bytes_sec = 0;
}

int main(int argc, char *argv[])
{  
    // Initialisere alle verdier
    pid_t   cpid;
    // Making our argument an int
    long    n_bytes = strtol(argv[1], &p, 10);
    int     pipefd[2];
    char    block_size[n_bytes];

    alarm(1);
    signal(SIGALRM, sig_handler);
    
    
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
            // Remove zombies
            wait(NULL);         
        }
        // In the parent process (READ)
        else 
        {
            //Parent process doesn't need this end of the pipe
            close(pipefd[1]);
            // Read returnerer antall bytes lest
            n_bytes = read(pipefd[0], argv[1], n_bytes);
            // To get total amount of bytes read
            tot_bytes_sec += n_bytes;
        }
        
    }
    return(0);
}