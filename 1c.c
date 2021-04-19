#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>

int     n_bytes; 
char*   p;
int     bandwidth = 0; 
int     tot_bytes = 0;
pid_t   ppid;

void sig_handler_2(int signum)
{
    printf("Total number of bytes through the pipe: %d bytes\n", tot_bytes);
}

void sig_handler_1(int signum)
{
    printf("Bandwidth: %d bytes/sec\n", bandwidth);
    alarm(1);
    bandwidth = 0;
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
    signal(SIGALRM, sig_handler_1);
    signal(SIGUSR1, sig_handler_2);         

    
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
            ppid = getppid();
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
            bandwidth += n_bytes;
            tot_bytes += n_bytes;
            
        }
        
    }
    return(0);
}