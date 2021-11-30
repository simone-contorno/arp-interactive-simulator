#include <stdio.h> // printf
#include <string.h> // strlen
#include <fcntl.h> // O_WRONLY
#include <sys/stat.h> // PIPE
#include <sys/types.h> // PIPE
#include <unistd.h> // write and close
#include <stdlib.h> // EXIT_SUCCESS
#include <signal.h> 

#define TIMER 60

int timer = TIMER;

static void signal_handler(int sig){
    if (sig == SIGUSR1) {
        //printf("The process Command console is still active.\n"); fflush(stdout);
        timer = TIMER;
        sleep(1);
    }
    else if (sig == SIGUSR2) {
        //printf("The process Inspection console is still active.\n"); fflush(stdout);
        timer = TIMER;
        sleep(1);
    }
    else if (sig == SIGBUS) {
        //printf("The process Motor x is still active.\n"); fflush(stdout);
        timer = TIMER;
        sleep(1);
    }
    else if (sig == SIGPROF) {
        //printf("The process Motor z is still active.\n"); fflush(stdout);
        timer = TIMER;
        sleep(1);
    } 
    else if (sig == SIGINT) {
        exit(EXIT_SUCCESS);
    }
}

int main(){
    /*
     * 1. Open PID.txt file that contain the pid for each process
     * 2. Write the pid of this one
     * 3. Read the pids of other processes
     */
    // 1.
    char *filename = "/tmp/PID.txt";
    FILE *file = fopen (filename, "a");
    int pid = getpid();

    // 2.
    fprintf(file, "%d\n", pid);
    fclose(file);

    sleep(1);

    printf("Whatchdog is running...\nTimer: %i [s]\n", TIMER);
    fflush(stdout);
    
    // 3.
    FILE *file_r = fopen (filename, "r");
    int str[5]; 
    fscanf(file_r, "%d %d %d %d", &str[0], &str[1], &str[2], &str[3]);

    pid_t cc = str[0];
    pid_t ic = str[1];
    pid_t mx = str[2];
    pid_t mz = str[3];

    while (1){
        // Signal from the Command console
        signal(SIGUSR1, signal_handler);
        // Signal from the Inspection Console
        signal(SIGUSR2, signal_handler);
        // Signal from the Motor x
        signal(SIGBUS, signal_handler);
        // Signal from the Motor z
        signal(SIGPROF, signal_handler);
        // Signal from the Command console to quit
        signal(SIGINT, signal_handler);
    
        if(timer == 0) {
            timer = TIMER;
            // Send signal to the Command console
            kill(cc, SIGALRM);
            // Send signal to the Inspection console
            kill(ic, SIGALRM);
            // Send signal to the Motor x 
            kill(mx, SIGALRM);
            // Send signal to the Motor z
            kill(mz, SIGALRM);
        }

        sleep(1);
        if (timer % 5 == 0)
            printf("Timer: %i\n", timer);
        timer--;
    }
     
    return 0;
}