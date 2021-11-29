mkdir executables
gcc src/process1/command_console.c -o executables/command_console &
gcc src/process2/inspection_console.c -o executables/inspection_console &
gcc src/process3/motor_x.c -o executables/motor_x &
gcc src/process4/motor_z.c -o executables/motor_z &
gcc src/process5/watchdog.c -o executables/watchdog &
