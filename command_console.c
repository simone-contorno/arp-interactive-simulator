#include <stdio.h> // printf
#include <string.h> // strlen
#include <fcntl.h> // O_WRONLY
#include <sys/stat.h> // PIPE
#include <sys/types.h> // PIPE
#include <unistd.h> // write and close
#include <stdlib.h> // EXIT_SUCCESS

int main() {
    // Variables
    int fd_x;
    int fd_z;
    char command[80];

    // PIPE 
    char * myfifo_x = "/tmp/myfifo_x"; 
    mkfifo(myfifo_x, 0666);
    //char * myfifo_z = "/tmp/myfifo_z"; 
    //mkfifo(myfifo_z, 0666);
    char * myfifo_xi = "/tmp/myfifo_xi"; 
    mkfifo(myfifo_xi, 0666);
    char * myfifo_zi = "/tmp/myfifo_zi"; 
    mkfifo(myfifo_zi, 0666); 
    char * myfifo_xiw = "/tmp/myfifo_xiw"; 
    mkfifo(myfifo_xiw, 0666); 

    while (1) {
        // Open PIPE
        fd_x = open(myfifo_x, O_WRONLY); 
        //fd_z = open(myfifo_z, O_WRONLY); 

        // Get command
        printf("Commands:\n1 - X axis increase\n2 - X axis decrease\n3 - X axis stop\n4 - Z axis increase\n5 - Z axis decrease\n6 - Z axis stop\nCommand: "); fflush(stdout);
        fgets(command, 80, stdin); 

        // Send command to the motors
        if (1 <= atoi(command) <= 3) 
            write(fd_x, command, strlen(command)+1); 
        //else if (4 <= atoi(command) <= 6) 
            //write(fd_z, command, strlen(command)+1); 
        else  
            printf("Command not valid, insert and integer value between 1 and 6 or 0 to quit.\n"); fflush(stdout);
        
        // Exit
        if (atoi(command) == 0) {
            write(fd_x, command, strlen(command)+1); 
            //write(fd_z, command, strlen(command)+1); 
            exit(EXIT_SUCCESS);
        }

        // Close PIPE
        close(fd_x); 
        //sleep(1);
        //close(fd_z); 
        //sleep(1);
    }
    return 0;
}