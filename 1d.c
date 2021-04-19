// SPØR: Hva er hovedforskjellen mellom å bruke named pipes og pipes?

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


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
    char    block_size[n_bytes];
    int fd;
    
    // Specify filename
    char *pathname = argv[2];

    alarm(1); 

    // Create pipe
    if((mkfifo(pathname, 0666)) < 0)
    {
        perror("Piped failed :/");
        exit(EXIT_FAILURE);
    }

    // Opening file
    if((fd = open(pathname, O_RDWR))<0)
    {
        printf("Child: Error opening FIFO in %u.\n", getpid());
        exit(EXIT_FAILURE);
    } 

    // Split into two processes
    if ((cpid = fork()) == -1) 
    {
        perror("Fork failed</3 (so sad :'( ))");
        exit(EXIT_FAILURE);
    }
    
    while(1)
    {
        if (cpid == 0)
        {   
            ppid = getppid();

            write(fd, block_size, sizeof(block_size));
            
        }
        // In the parent process (READ)
        else 
        {
            signal(SIGALRM, sig_handler_1);
            signal(SIGUSR1, sig_handler_2); 
            n_bytes = read(fd, block_size, sizeof(block_size));
            // To get total amount of bytes read
            bandwidth += n_bytes;
            tot_bytes += n_bytes;
        } 
    }
     
    return(0);
}