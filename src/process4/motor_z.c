#include <stdio.h> // printf
#include <string.h> // strlen
#include <fcntl.h> // O_WRONLY
#include <sys/stat.h> // PIPE
#include <sys/types.h> // PIPE
#include <unistd.h> // write and close
#include <stdlib.h> // EXIT_SUCCESS
#include <signal.h>
#include <time.h>

#define Z_MIN 0
#define Z_MAX 100
#define SPEED_MIN 0
#define SPEED_MAX 10

// Global variables
int fd_z, fd_zi;
char format_string_1[80] = "%d";
char format_string_2[80] = "%f";
char command_c[80], command_i[80];
char position[80] = {'0'};
double pos_z;
int number;
int speed = 0;
int max_x = 60;
int str[5]; 
int pos_flag = 1;

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
        sleep(2);

        fd_zi = open(myfifo_zi, O_WRONLY); 

        // Exit
        if (command_c[0] == 'q') {
            write(fd_zi, command_c, strlen(command_c)+1); 
            close(fd_zi);
            exit(EXIT_SUCCESS);
        }

        sscanf(command_c, format_string_1, &number);
  
        // Exec task
        switch (number) {
            case 4: speed += 1; break;
            case 5: speed -= 1; break;
            case 6: speed = 0; break;            
            default: break;
        }
        if (speed > SPEED_MAX) {
            printf("Speed max reached!!\n"); fflush(stdout); 
            speed = SPEED_MAX;
        }

        // Send position to the Inspection console
        sscanf(position, format_string_2, &pos_z);
        srand(time(0));
        int r = rand() % 10;
        double error = (int)r / 100.0;
        pos_z += speed + error;
        if (pos_z < Z_MIN) {
            pos_z = Z_MIN;
            speed = SPEED_MIN;
        }
        else if (pos_z > Z_MAX) {
            pos_z = Z_MAX;
            speed = SPEED_MIN;
        }
        sprintf(position, format_string_2, pos_z);
        write(fd_zi, position, strlen(position)+1);
        printf("Speed: %i\n", speed); fflush(stdout); 
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
            pos_flag = 1;
            if (pos_z != 0)
                speed = -5;
            sprintf(position, format_string_2, pos_z);
            printf("Speed: %i\n", speed); fflush(stdout); 
            printf("Position: %s\n", position); fflush(stdout); 
        }

        // Close PIPE
        close(fd_zi);
        sleep(1);
    }

    else if (sig == SIGALRM) {
        pos_flag = 1;
        printf("Watchdog alarm detected: reset speed and position!!\n"); fflush(stdout);
        if (pos_z != 0)
            speed = -5;
        sprintf(position, format_string_2, pos_z);
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

    sleep(2);

    printf("Motor z is running...\n\n");

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
        // Read signal from the command console to exit
        signal(SIGINT, signal_handler);
        // Read signal from the inspection console to stop
        signal(SIGBUS, signal_handler);

        sleep(10);
        if (speed != 0) {
            srand(time(0));
            int r = rand() % 10;
            double error = (int)r / 100.0;
            pos_z += speed + error;
        }
        else 
            pos_z += speed;
        if (pos_z < Z_MIN) {
            pos_z = Z_MIN;
            speed = SPEED_MIN;
        }
        else if (pos_z > Z_MAX) {
            pos_z = Z_MAX;
            speed = SPEED_MIN;
        }
        // Take pids
        pid_t ic = str[1];
        pid_t wd = str[4];

        // Send signals
        if (speed != 0 || pos_flag == 1) {
            kill(wd, SIGBUS); // to the watchdog
            kill(ic, SIGUSR2); // to the inspection console

            fd_zi = open(myfifo_zi, O_WRONLY); 

            sprintf(position, format_string_2, pos_z);
            write(fd_zi, position, strlen(position)+1);

            printf("Update position: %s\n\n", position); 
            fflush(stdout);

            close(fd_zi);

            if (speed >= 0)
                pos_flag = 0;
        }            
    }

    return 0;
}