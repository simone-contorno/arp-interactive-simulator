#include <stdio.h> // printf
#include <string.h> // strlen
#include <fcntl.h> // O_WRONLY
#include <sys/stat.h> // PIPE
#include <sys/types.h> // PIPE
#include <unistd.h> // write and close
#include <stdlib.h> // EXIT_SUCCESS

int main() {
    // Variables
    int fd_xi, fd_zi;
    char position_x[80], position_z[80], command[80];
    int flag = 0;

    // PIPE 
    char * myfifo_xi = "/tmp/myfifo_xi"; 
    //sleep(1);
    char * myfifo_zi = "/tmp/myfifo_zi"; 
    //sleep(1);

    if (fork() == 0) { /* --- Writing to Motor X and Moror Z --- */
        while(1) {
            if (flag == 1)
                exit(EXIT_SUCCESS);

            // Open PIPE to send a Reset or a Stop signal
            fd_xi = open(myfifo_xi, O_WRONLY); 
            //fd_zi = open(myfifo_zi, O_WRONLY); 

            // Get command
            printf("Commands:\nR - Reset\nS - Stop\nCommand: "); fflush(stdout);
            fgets(command, 80, stdin); 

            // Send command
            if (command[0] == 'R' || command[0] == 'S') {
                write(fd_xi, command, strlen(command)+1); 
                //write(fd_zi, command, strlen(command)+1); 
            }
            else
                printf("Command not valid, insert and integer value between 1 and 6 or 0 to quit.\n"); fflush(stdout);

            // Close PIPE
            close(fd_xi); 
            //sleep(1);
            //close(fd_zi); 
            //sleep(1);
        }
    }
    else {
        while (1) {
            // Open PIPE
            fd_xi = open(myfifo_xi, O_RDONLY); 
            //fd_zi = open(myfifo_zi, O_RDONLY); 

            // Get commands
            read(fd_xi, position_x, 80);
            //read(fd_zi, position_z, 80);

            // Exit
            if (position_x[0] == 'q' || position_z[0] == 'q') {
                flag = 1;
                exit(EXIT_SUCCESS);
            }

            // Print positions
            printf("\nX position: %s \n", position_x);
            //printf("Z position: %i \n", atoi(position_z));

            // Close PIPE
            close(fd_xi); 
            //sleep(1);
            //close(fd_zi); 
            //sleep(1);
        }
    }
    return 0;
}