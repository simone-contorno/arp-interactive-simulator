#include <stdio.h> // printf
#include <string.h> // strlen
#include <fcntl.h> // O_WRONLY
#include <sys/stat.h> // PIPE
#include <sys/types.h> // PIPE
#include <unistd.h> // write and close
#include <stdlib.h> // EXIT_SUCCESS

int main() {
    // Variables
    int fd_x, fd_xi;
    char format_string[80] = "%d";
    char command_c[80], command_i[80], position[80];
    int number;
    int flag = 0;
    int speed = 0;
    int max_x = 60;

    // PIPE 
    char * myfifo_x = "/tmp/myfifo_x"; 
    //sleep(1);
    char * myfifo_xi = "/tmp/myfifo_xi"; 
    //sleep(1);

    if (fork() == 0) { /* Reading from Command console and Writing to Inspection console */
        while (1) {           
            // Open PIPE
            fd_x = open(myfifo_x, O_RDONLY); 
                
            // Get commands
            read(fd_x, command_c, 80); 
            printf("Command: %s", command_c);
            
            close(fd_x);
            sleep(1);
            
            fd_xi = open(myfifo_xi, O_WRONLY); 

            // Exit
            if (command_c[0] == 'q') {
                write(fd_xi, command_c, strlen(command_c)+1); 
                flag = 1;
                exit(EXIT_SUCCESS);
            }

            sscanf(command_c, format_string, &number);
            printf("Number: %i\n", number);

            // Exec task
            switch (number) {
                case 1: speed += 1; break;
                case 2: speed -= 1; break;
                case 3: speed = 0; break;            
                default: break;
            }
            printf("Speed: %i\n", speed);

            // Send position to the Inspection console
            sscanf(position, format_string, &number);
            number += speed;
            sprintf(position, format_string, number);
            write(fd_xi, position, strlen(position)+1);
            printf("Position: %s\n\n", position);
                
            // Close PIPE
            close(fd_xi);
            //sleep(1);
        }
    }
    else {
        while (1) { /* Reading from Inspection console */
            if (flag == 1) 
                exit(EXIT_SUCCESS);
            
            // Open PIPE
            fd_xi = open(myfifo_xi, O_RDONLY);
            
            // Get commands
            read(fd_xi, command_i, 80); 

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
            close(fd_xi);
            //sleep(1);
        }
    }
    return 0;
}