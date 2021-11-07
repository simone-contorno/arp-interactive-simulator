#include <stdio.h> // printf
#include <string.h> // strlen
#include <fcntl.h> // O_WRONLY
#include <sys/stat.h> // PIPE
#include <sys/types.h> // PIPE
#include <unistd.h> // write and close
#include <stdlib.h> // EXIT_SUCCESS

int main() {
    // Variables
    int fd_z, fd_itoz, fd_ztoi;
    char command_c[80], command_i[80], position[80];
    int speed = 0;
    int max_x = 60;

    // PIPE 
    char * myfifo_z = "/tmp/myfifo_z"; 
    mkfifo(myfifo_z, 0666); 
    char * myfifo_itoz = "/tmp/myfifo_itoz"; 
    mkfifo(myfifo_itoz, 0666); 
    char * myfifo_ztoi = "/tmp/myfifo_ztoi"; 
    mkfifo(myfifo_ztoi, 0666); 

    while (1) {
        /* --- Reading from Inspection console --- */

        // Open PIPE
        fd_itoz = open(myfifo_itoz, O_RDONLY);
        
        // Get commands
        read(fd_itoz, command_i, 80); 

        // Exec command
        if (command_i == "R") {
            position[80] = 0;
            speed = 0;
        }
        else if (command_i == "S") 
            speed = 0;

        // Close PIPE
        close(fd_itoz);
        unlink(myfifo_itoz);
        sleep(5);

        /* --- Reading from Command console and Writing to Inspection console --- */

        // Open PIPE
        fd_z = open(myfifo_z, O_RDONLY); 
        fd_ztoi = open(myfifo_ztoi, O_WRONLY); 
        
        // Get commands
        read(fd_z, command_c, 80); 

        // Exit
        if (atoi(command_c) == 0) {
            command_c[0] = '-';
            write(fd_ztoi, command_c, strlen(command_c)+1); 
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
        write(fd_ztoi, position, strlen(position)+1);

        // Close PIPE
        close(fd_z);
        close(fd_ztoi);
        unlink(myfifo_z);
        unlink(myfifo_ztoi);
        sleep(5);
    }
    return 0;
}