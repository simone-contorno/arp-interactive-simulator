#include <stdio.h> // printf
#include <string.h> // strlen
#include <fcntl.h> // O_WRONLY
#include <sys/stat.h> // PIPE
#include <sys/types.h> // PIPE
#include <unistd.h> // write and close
#include <stdlib.h> // EXIT_SUCCESS
#include <signal.h>

// Global variables
int fd_z, fd_zi;
char format_string[80] = "%d";
char command_c[80], command_i[80];
char position[80] = {'0'};
int number;
int speed = 0;
int max_x = 60;
int str[5]; 

// PIPE 
char * myfifo_z = "/tmp/myfifo_z"; 
char * myfifo_zi = "/tmp/myfifo_zi"; 

static void signal_handler(int sig) {
    if (sig == SIGUSR1) { // Command console 
        fd_z = open(myfifo_z, O_RDONLY); 
                
        // Get command
        read(fd_z, command_c, 80); 
        printf("Command: %s", command_c); fflush(stdout);    
        close(fd_z);

        // Take pids
        pid_t ic = str[1];
        pid_t wd = str[4];

        // Send signals
        kill(wd, SIGPROF); // to the watchdog
        kill(ic, SIGUSR2); // to the inspection console
        sleep(1);

        fd_zi = open(myfifo_zi, O_WRONLY); 

        // Exit
        if (command_c[0] == 'q') {
            write(fd_zi, command_c, strlen(command_c)+1); 
            exit(EXIT_SUCCESS);
        }

        sscanf(command_c, format_string, &number);
        printf("Number: %i\n", number); fflush(stdout); 

        // Exec task
        switch (number) {
            case 4: speed += 1; break;
            case 5: speed -= 1; break;
            case 6: speed = 0; break;            
            default: break;
        }
        printf("Speed: %i\n", speed); fflush(stdout); 

        // Send position to the Inspection console
        sscanf(position, format_string, &number);
        number += speed;
        sprintf(position, format_string, number);
        write(fd_zi, position, strlen(position)+1);
        printf("Position: %s\n\n", position); fflush(stdout); 

        // Close PIPE
        close(fd_zi);
        sleep(1);
    }

    else if (sig == SIGUSR2) { // Inspection console
        // Open PIPE
        fd_zi = open(myfifo_zi, O_RDONLY);
            
        // Get commands
        read(fd_zi, command_i, 80); 

        // Take pid
        pid_t wd = str[4];

        // Send signal to the watchdog
        kill(wd, SIGPROF);

        // Exec command
        if (command_i[0] == 'R') {
            number = 0;
            sprintf(position, format_string, number);
            speed = 0;
            printf("Position: %s\n", position);
            printf("Speed: %i\n", speed);
        }
        else if (command_i[0] == 'S') {
            speed = 0;
            printf("Speed: %i\n", speed);
        }

        // Close PIPE
        close(fd_zi);
        sleep(1);
    }

    else if (sig == SIGALRM) {
        printf("Watchdog alarm detected: reset speed and position!!\n"); fflush(stdout);
        number = 0;
        sprintf(position, format_string, number);
        speed = 0;
        printf("Position: %s\n", position); fflush(stdout);
        printf("Speed: %i\n\n", speed);
        sleep(1);
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

    sleep(5);

    // 3.
    FILE *file_r = fopen (filename, "r");
    fscanf(file_r, "%d %d %d %d %d", &str[0], &str[1], &str[2], &str[3], &str[4]);

    while(1) {
        // Read signal from the command console
        signal(SIGUSR1, signal_handler);
        // Read signal from the inspection console
        signal(SIGUSR2, signal_handler);
        // Read signal from the watchdog
        signal(SIGALRM, signal_handler);

        sleep(10);
        number += speed;
        sprintf(position, format_string, number);
        printf("Update position: %s\n\n", position); 
        fflush(stdout);
    }

    return 0;
}