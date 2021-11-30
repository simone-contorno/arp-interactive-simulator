#include <stdio.h> // printf
#include <string.h> // strlen
#include <fcntl.h> // O_WRONLY
#include <sys/stat.h> // PIPE
#include <sys/types.h> // PIPE
#include <unistd.h> // write and close
#include <stdlib.h> // EXIT_SUCCESS
#include <signal.h>
#include <time.h>

#define X_MIN 0
#define X_MAX 100
#define SPEED_MIN 0
#define SPEED_MAX 10

// Global variables
int fd_x, fd_xi;
char format_string_1[80] = "%d";
char format_string_2[80] = "%f";
char command_c[80], command_i[80], position[80];
double pos_x;
int number;
int speed = 0;
int max_x = 60;
int str[5]; 
int pos_flag = 1;

// PIPE 
char * myfifo_x = "/tmp/myfifo_x"; 
char * myfifo_xi = "/tmp/myfifo_xi"; 

static void signal_handler(int sig) {
    if (sig == SIGUSR1) { // Command console 
        fd_x = open(myfifo_x, O_RDONLY); 
                
        // Get command
        read(fd_x, command_c, 80); 
        printf("Command: %s", command_c); fflush(stdout);    
        close(fd_x);

        // Take pids
        pid_t ic = str[1];
        pid_t wd = str[4];

        // Send signals
        kill(wd, SIGBUS); // to the watchdog
        kill(ic, SIGUSR1); // to the inspection console
        sleep(1);

        fd_xi = open(myfifo_xi, O_WRONLY); 
        sscanf(command_c, format_string_1, &number);

        // Exec task
        switch (number) {
            case 1: speed += 1; break;
            case 2: speed -= 1; break;
            case 3: speed = 0; break;            
            default: break;
        }
        if (speed > SPEED_MAX) {
            printf("Speed max reached!!\n"); fflush(stdout); 
            speed = SPEED_MAX;
        }

        // Send position to the Inspection console
        sscanf(position, format_string_2, &pos_x);
        int r = rand() % 10;
        double error = (int)r / 100.0;
        pos_x += speed + error;
        if (pos_x < X_MIN) {
            pos_x = X_MIN;
            speed = SPEED_MIN;
        }
        else if (pos_x > X_MAX) {
            pos_x = X_MAX;
            speed = SPEED_MIN;
        }
        sprintf(position, format_string_2, pos_x);
        write(fd_xi, position, strlen(position)+1);
        printf("Speed: %i\n", speed); fflush(stdout); 
        printf("Position: %s\n\n", position); fflush(stdout); 

        // Close PIPE
        close(fd_xi);
        sleep(1);
    }

    else if (sig == SIGUSR2) {
        // Open PIPE
        fd_xi = open(myfifo_xi, O_RDONLY);
            
        // Get commands
        read(fd_xi, command_i, 80); 

        // Take pid
        pid_t wd = str[4];

        // Send signal to the watchdog
        kill(wd, SIGBUS);

        // Exec command
        if (command_i[0] == 'R') {
            pos_flag = 1;
            if (pos_x != 0)
                speed = -5;
            sprintf(position, format_string_2, pos_x);
            printf("Speed: %i\n", speed); fflush(stdout); 
            printf("Position: %s\n", position); fflush(stdout); 

        }

        // Close PIPE
        close(fd_xi);
        sleep(1);
    }

    else if (sig == SIGALRM) {
        pos_flag = 1;
        printf("Watchdog alarm detected: reset speed and position!!\n"); fflush(stdout);
        if (pos_x != 0)
            speed = -5;
        sprintf(position, format_string_2, pos_x);
        printf("Speed: %i\n", speed); fflush(stdout);
        printf("Position: %s\n\n", position); fflush(stdout);
        sleep(1);
    }

    else if (sig == SIGBUS) {
        speed = 0;
        printf("\nStop signal detected!\nSpeed: %i\n", speed); fflush(stdout); 
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

    sleep(3);

    printf("Motor x is running...\n\n");

    // 3.
    FILE *file_r = fopen (filename, "r");
    fscanf(file_r, "%d %d %d %d %d", &str[0], &str[1], &str[2],&str[3],&str[4]);

    while(1) {
        // Read signal from the command console
        signal(SIGUSR1, signal_handler);
        // Read signal from the inspection console
        signal(SIGUSR2, signal_handler);
        // Read signal from the watchdog
        signal(SIGALRM, signal_handler);
        // Read signal from the command console to exit
        signal(SIGINT, signal_handler);
        // Read signal from the inspection console to stop
        signal(SIGBUS, signal_handler);

        sleep(10);
        if (speed != 0) {
            srand(time(0));
            int r = rand() % 10;
            double error = (int)r / 100.0;
            pos_x += speed + error;
        }
        else 
            pos_x += speed;
        if (pos_x < X_MIN) {
            pos_x = X_MIN;
            speed = SPEED_MIN;
        }
        else if (pos_x > X_MAX) {
            pos_x = X_MAX;
            speed = SPEED_MIN;
        }
        // Take pids
        pid_t ic = str[1];
        pid_t wd = str[4];

        // Send signals
        if (speed != 0 || pos_flag == 1) {
            kill(wd, SIGBUS); // to the watchdog
            kill(ic, SIGUSR1); // to the inspection console

            fd_xi = open(myfifo_xi, O_WRONLY); 

            sprintf(position, format_string_2, pos_x);
            write(fd_xi, position, strlen(position)+1);
        
            printf("Update position: %s\n\n", position); 
            fflush(stdout);

            close(fd_xi);

            if (speed >= 0)
                pos_flag = 0;
        }
    }

    return 0;
}