#include <stdio.h> // printf
#include <string.h> // strlen
#include <fcntl.h> // O_WRONLY
#include <sys/stat.h> // PIPE
#include <sys/types.h> // PIPE
#include <unistd.h> // write and close
#include <stdlib.h> // EXIT_SUCCESS
#include <signal.h>

static void signal_handler(int sig) {
    if (sig == SIGALRM){
        printf("\n\nWatchdog alarm detected, motors are been reset!!\n\nCommand: ");
        fflush(stdout);
    }
}

int main() {
    // Variables
    int fd_x;
    int fd_z;
    char command[80];

    /*
     * 1. Open PID.txt file that contain the pid for each process
     * 2. Write the pid of this one
     * 3. Read the pids of other processes
     */
    // 1.
    char *filename = "/tmp/PID.txt";
    FILE *file = fopen (filename, "w");
    int pid = getpid();

    // 2.
    fprintf(file, "%d\n", pid);
    fclose(file);

    sleep(5);

    // 3.
    FILE *file_r = fopen (filename, "r");
    int str[5]; 
    fscanf(file_r, "%d %d %d %d %d", &str[0], &str[1], &str[2], &str[3], &str[4]);
    
    // Take pids
    pid_t ic = str[1]; // inspection console
    pid_t mx = str[2]; // motor_x
    pid_t mz = str[3]; // motor_z
    pid_t wd = str[4]; // watchdog

    // PIPE 
    char * myfifo_x = "/tmp/myfifo_x"; 
    mkfifo(myfifo_x, 0666);
    char * myfifo_z = "/tmp/myfifo_z"; 
    mkfifo(myfifo_z, 0666);
    char * myfifo_xi = "/tmp/myfifo_xi"; 
    mkfifo(myfifo_xi, 0666);
    char * myfifo_zi = "/tmp/myfifo_zi"; 
    mkfifo(myfifo_zi, 0666); 

    printf("Commands:\n1 - X axis increase\n2 - X axis decrease\n3 - X axis stop\n4 - Z axis increase\n5 - Z axis decrease\n6 - Z axis stop\nPress 'q' to quit\n"); fflush(stdout);

    while (1) { 
        // Wait for signal from the watchdog
        signal(SIGALRM, signal_handler);

        printf("Command: "); 
        fflush(stdout);

        // Get command
        fgets(command, 80, stdin); 
        kill(wd, SIGUSR1);

        // Send command to the motors
        if (command[0] == '1' || command[0] == '2' || command[0] == '3') {
            // Send signal to motor x
            kill(mx, SIGUSR1);
            sleep(1);

            // Open PIPE
            fd_x = open(myfifo_x, O_WRONLY); 
            write(fd_x, command, strlen(command)+1); 
        }
        else if (command[0] == '4' || command[0] == '5' || command[0] == '6') {
            // Send signal to motor z
            kill(mz, SIGUSR1);
            sleep(1);
            
            // Open PIPE
            fd_z = open(myfifo_z, O_WRONLY); 
            write(fd_z, command, strlen(command)+1); 
        }
        else {
            printf("Command not valid, insert and integer value between 1 and 6 or type 'q' to quit.\n"); 
            fflush(stdout);
        }

        // Exit
        if (command[0] == 'q') {
            kill(wd, SIGINT);
            kill(mx, SIGINT);
            kill(mz, SIGINT);
            kill(ic, SIGINT);

            unlink(myfifo_x);
            unlink(myfifo_z);
            unlink(myfifo_xi);
            unlink(myfifo_zi);
            remove(filename);

            exit(EXIT_SUCCESS);
        }

        // Close PIPE
        close(fd_x); 
        close(fd_z);
    }
    return 0;
}