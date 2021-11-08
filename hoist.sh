gcc command_console.c -o command_console &
gcc -pthread inspection_console.c -o inspection_console &
gcc -pthread motor_x.c -o motor_x &
#gcc motor_z.c -o motor_z &

konsole  -e ./command_console &
konsole  -e ./inspection_console &
konsole  -e ./motor_x &
#konsole  -e ./motor_z &