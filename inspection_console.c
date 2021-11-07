#include <stdio.h> // printf
#include <string.h> // strlen
#include <fcntl.h> // O_WRONLY
#include <sys/stat.h> // PIPE
#include <sys/types.h> // PIPE
#include <unistd.h> // write and close
#include <stdlib.h> // EXIT_SUCCESS

int main() {
    // Variables
    int fd_xtoi, fd_ztoi, fd_itox, fd_itoz;
    char position_x[80];
    char position_z[80];
    char command[80];

    // PIPE 
    char * myfifo_xtoi = "/tmp/myfifo_xtoi"; 
    mkfifo(myfifo_xtoi, 0666);
    char * myfifo_ztoi = "/tmp/myfifo_ztoi"; 
    mkfifo(myfifo_ztoi, 0666); 
    char * myfifo_itox = "/tmp/myfifo_itox"; 
    mkfifo(myfifo_itox, 0666);
    char * myfifo_itoz = "/tmp/myfifo_itoz"; 
    mkfifo(myfifo_itoz, 0666);   

    while (1) {
        // Open PIPE
        fd_xtoi = open(myfifo_xtoi, O_RDONLY); 
        fd_ztoi = open(myfifo_ztoi, O_RDONLY); 

        // Get commands
        read(fd_xtoi, position_x, 80);
        read(fd_ztoi, position_z, 80);
        
        // Print positions
        printf("X position: %i", atoi(position_x));
        printf("Z position: %i", atoi(position_z));

        // Close PIPE
        close(fd_xtoi); 
        close(fd_ztoi); 

        // Exit
        if (position_x[0] == '-' || position_z[0] == '-') 
            exit(EXIT_SUCCESS);

        // Open PIPE to send a Reset or a Stop signal
        fd_itox = open(myfifo_xtoi, O_WRONLY); 
        fd_itoz = open(myfifo_ztoi, O_WRONLY); 

        // Get command
        printf("Commands:\nR - Reset\nS - Stop\nCommand: "); fflush(stdout);
        fgets(command, 80, stdin); 

        // Send command
        if (command == "R" || command == "S") {
            write(fd_itox, command, strlen(command)+1); 
            write(fd_itoz, command, strlen(command)+1); 
        }
        else
            printf("Command not valid, insert and integer value between 1 and 6 or 0 to quit.\n"); fflush(stdout);

        // Close PIPE
        close(fd_itox); 
        close(fd_itoz); 
    }
    return 0;
}