#include <stdio.h> // printf
#include <string.h> // strlen
#include <fcntl.h> // O_WRONLY
#include <sys/stat.h> // PIPE
#include <sys/types.h> // PIPE
#include <unistd.h> // write and close
#include <stdlib.h> // EXIT_SUCCESS

int main() {
    // Variables
    int fd_x, fd_itox, fd_xtoi;
    char command_c[80], command_i[80], position[80];
    int speed = 0;
    int max_x = 60;

    // PIPE 
    char * myfifo_x = "/tmp/myfifo_x"; 
    mkfifo(myfifo_x, 0666); 
    char * myfifo_itox = "/tmp/myfifo_itox"; 
    mkfifo(myfifo_itox, 0666); 
    char * myfifo_xtoi = "/tmp/myfifo_xtoi"; 
    mkfifo(myfifo_xtoi, 0666); 

    while (1) {
        /* --- Reading from Inspection console --- */

        // Open PIPE
        fd_itox = open(myfifo_itox, O_RDONLY);
        
        // Get commands
        read(fd_itox, command_i, 80); 

        // Exec command
        if (command_i == "R") {
            position[80] = 0;
            speed = 0;
        }
        else if (command_i == "S") 
            speed = 0;

        // Close PIPE
        close(fd_itox);
        unlink(myfifo_itox);
        sleep(5);

        /* --- Reading from Command console and Writing to Inspection console --- */

        // Open PIPE
        fd_x = open(myfifo_x, O_RDONLY); 
        fd_xtoi = open(myfifo_xtoi, O_WRONLY); 
        
        // Get commands
        read(fd_x, command_c, 80); 

        // Exit
        if (atoi(command_c) == 0) {
            command_c[0] = '-';
            write(fd_xtoi, command_c, strlen(command_c)+1); 
            exit(EXIT_SUCCESS);
        }

        // Exec task
        switch (atoi(command_c)) {
            case 1: speed += 1; break;
            case 2: speed -= 1; break;
            case 3: speed = 0; break;            
            default: break;
        }
        command_c[80];

        // Send position to the Inspection console
        position[80] += atoi(position) + 1;
        write(fd_xtoi, position, strlen(position)+1);

        // Close PIPE
        close(fd_x);
        close(fd_xtoi);
        unlink(myfifo_x);
        unlink(myfifo_xtoi);
        sleep(5);
    }
    return 0;
}