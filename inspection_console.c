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

        // Get commands
        read(fd_xi, position_x, 80);

        // Take pid
        pid_t wd = str[4];

        // Send signal to the watchdog
        kill(wd, SIGUSR2);

        // Exit
        if (position_x[0] == 'q') {
            close(fd_xi);
            exit(EXIT_SUCCESS);
        }

        // Print position
        printf("\nX position: %s \n", position_x); fflush(stdout);
        printf("Command: "); fflush(stdout);

        // Close PIPE
        close(fd_xi); 
    }

    else if (sig == SIGUSR2) {
        // Open PIPE
        fd_zi = open(myfifo_zi, O_RDONLY); 

        // Get commands
        read(fd_zi, position_z, 80);

        // Take pid
        pid_t wd = str[4];

        // Send signal to the watchdog
        kill(wd, SIGUSR2);

        // Exit
        if (position_z[0] == 'q') {
            close(fd_zi); 
            exit(EXIT_SUCCESS);
        }

        // Print position
        printf("\nZ position: %s \n", position_z); fflush(stdout);
        printf("Command: "); fflush(stdout);

        // Close PIPE
        close(fd_zi); 
    }

    else if (sig == SIGALRM) {
        printf("\n\nWatchdog alarm detected, motors are been reset!!\n\nCommand: "); fflush(stdout);
    } 

    else if (sig == SIGINT) {
        exit(EXIT_SUCCESS);
    }
}

int main() {    
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

    sleep(4);

    // 3.
    FILE *file_r = fopen (filename, "r");
    fscanf(file_r, "%d %d %d %d %d", &str[0], &str[1], &str[2], &str[3], &str[4]);
    pid_t mx = str[2];
    pid_t mz = str[3];
    pid_t wd = str[4]; 

    printf("Commands:\nR - Reset\nS - Stop\n"); fflush(stdout);

    while(1) {
        // Read signal from motor x
        signal(SIGUSR1, signal_handler);
        // Read signal from motor z
        signal(SIGUSR2, signal_handler);
        // Read signal from the watchdog
        signal(SIGALRM, signal_handler);
        // Read signal from the command console to exit
        signal(SIGINT, signal_handler);

        printf("Command: ");

        // Get command
        fgets(command, 80, stdin); 

        // Send signal to the watchdog
        kill(wd, SIGUSR2);
       
        // Send command
        if (command[0] == 'R') {
            // Send signal to the motor x
            kill(mx, SIGUSR2);
            // Send signal to the motor z
            kill(mz, SIGUSR2);
            // Open PIPE to send a Reset or a Stop signal
            fd_xi = open(myfifo_xi, O_WRONLY); 
            fd_zi = open(myfifo_zi, O_WRONLY); 

            write(fd_xi, command, strlen(command)+1); 
            write(fd_zi, command, strlen(command)+1); 

            // Close PIPE
            close(fd_xi); 
            close(fd_zi); 
        }
        else if (command[0] == 'S') {
            // Send STOP signal to the motor x
            kill(mx, SIGBUS);
            // Send STOP signal to the motor z
            kill(mz, SIGBUS);
        }
        else
            printf("Command not valid, type 'R' to reset or 'S' to stop.\n"); fflush(stdout);

        // Close PIPE
        //close(fd_xi); 
        //close(fd_zi); 
    }

    return 0;
}