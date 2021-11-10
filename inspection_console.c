#include <stdio.h> // printf
#include <string.h> // strlen
#include <fcntl.h> // O_WRONLY
#include <sys/stat.h> // PIPE
#include <sys/types.h> // PIPE
#include <unistd.h> // write and close
#include <stdlib.h> // EXIT_SUCCESS
#include <signal.h> 

// GLobal variables
int fd_xi, fd_zi;
char position_x[80], position_z[80], command[80];
int str[5];

// PIPE 
char * myfifo_xi = "/tmp/myfifo_xi"; 
char * myfifo_zi = "/tmp/myfifo_zi"; 

static void signal_handler(int sig){
    if (sig == SIGUSR1) {     
        // Open PIPE
        fd_xi = open(myfifo_xi, O_RDONLY); 
        fd_zi = open(myfifo_zi, O_RDONLY); 

        // Get commands
        read(fd_xi, position_x, 80);
        read(fd_zi, position_z, 80);

        // Take pid
        pid_t wd = str[4];

        // Send signal to the watchdog
        kill(wd, SIGUSR2);

        // Exit
        if (position_x[0] == 'q' || position_z[0] == 'q') 
            exit(EXIT_SUCCESS);

        // Print positions
        printf("\nX position: %s \n", position_x); fflush(stdout);
        printf("Z position: %s \n", position_z); fflush(stdout);
        printf("Command: "); fflush(stdout);

        // Close PIPE
        close(fd_xi); 
        close(fd_zi); 
    }
    else if (sig == SIGALRM) {
        printf("\n\nWatchdog alarm detected, motors are been reset!!\n\nCommand: "); fflush(stdout);
    } 
}

int main() {
    char * myfifo_xi = "/tmp/myfifo_xi"; 
    mkfifo(myfifo_xi, 0666);
    char * myfifo_zi = "/tmp/myfifo_zi"; 
    mkfifo(myfifo_zi, 0666); 
    
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

    sleep(5);

    // 3.
    FILE *file_r = fopen (filename, "r");
    fscanf(file_r, "%d %d %d %d %d", &str[0], &str[1], &str[2],&str[3],&str[4]);
    pid_t mx = str[2];
    pid_t mz = str[3];
    pid_t wd = str[4]; 

    printf("Commands:\nR - Reset\nS - Stop\n"); fflush(stdout);

    while(1) {
        printf("Command: ");

        // Read signal from motor x
        signal(SIGUSR1, signal_handler);
        // Read signal from the watchdog
        signal(SIGALRM, signal_handler);

        // Get command
        fgets(command, 80, stdin); 

        // Send signal to the watchdog
        kill(wd, SIGUSR2);
        // Send signal to the motor x
        kill(mx, SIGUSR2);
        // Send signal to the motor z
        kill(mz, SIGUSR1);

        // Open PIPE to send a Reset or a Stop signal
        fd_xi = open(myfifo_xi, O_WRONLY); 
        fd_zi = open(myfifo_zi, O_WRONLY); 

        // Send command
        if (command[0] == 'R' || command[0] == 'S') {
            write(fd_xi, command, strlen(command)+1); 
            write(fd_zi, command, strlen(command)+1); 
        }
        else
            printf("Command not valid, insert and integer value between 1 and 6 or 0 to quit.\n"); fflush(stdout);

        // Close PIPE
        close(fd_xi); 
        close(fd_zi); 
    }

    return 0;
}