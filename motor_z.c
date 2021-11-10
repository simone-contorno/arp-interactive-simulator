#include <stdio.h> // printf
#include <string.h> // strlen
#include <fcntl.h> // O_WRONLY
#include <sys/stat.h> // PIPE
#include <sys/types.h> // PIPE
#include <unistd.h> // write and close
#include <stdlib.h> // EXIT_SUCCESS

int main() {
    // Variables
    int fd_z, fd_zi;
    char format_string[80] = "%i";
    char command_c[80], command_i[80], position[80];
    int number;
    int speed = 0;
    int max_x = 60;

    // PIPE 
    char * myfifo_z = "/tmp/myfifo_z"; 
    sleep(1);
    char * myfifo_zi = "/tmp/myfifo_zi"; 
    sleep(1);

    while (1) {
        /* --- Reading from Inspection console --- */

        // Open PIPE
        fd_zi = open(myfifo_zi, O_RDONLY);
        
        // Get commands
        read(fd_zi, command_i, 80); 
        //sscanf(command_i, format_string, number);

        // Exec command
        if (command_i[0] == 'R') {
            position[80] = 0;
            speed = 0;
        }
        else if (command_i[0] == 'S') 
            speed = 0;

        // Close PIPE
        close(fd_zi);
        //unlink(myfifo_xi);
        sleep(1);

        /* --- Reading from Command console and Writing to Inspection console --- */

        // Open PIPE
        fd_z = open(myfifo_z, O_RDONLY); 
        fd_zi = open(myfifo_zi, O_WRONLY); 
        
        // Get commands
        read(fd_z, command_c, 80); 
        
        // Exit
        if (command_c[0] == 'q') {
            write(fd_zi, command_c, strlen(command_c)+1); 
            exit(EXIT_SUCCESS);
        }

        sscanf(command_i, format_string, number);

        // Exec task
        switch (number) {
            case 1: speed += 1; break;
            case 2: speed -= 1; break;
            case 3: speed = 0; break;            
            default: break;
        }
        number = 0;

        // Send position to the Inspection console
        sscanf(position, format_string, number);
        number += speed;
        sprintf(position, format_string, number);
        write(fd_zi, position, strlen(position)+1);

        // Close PIPE
        close(fd_z);
        unlink(myfifo_z);
        sleep(7);
        close(fd_zi);
        unlink(myfifo_zi);
        sleep(7);
    }
    return 0;
}