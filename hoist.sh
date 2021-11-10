gcc command_console.c -o command_console &
gcc inspection_console.c -o inspection_console &
gcc motor_x.c -o motor_x &
gcc motor_z.c -o motor_z &
gcc watchdog.c -o watchdog &

konsole  -e ./command_console &
sleep 1
konsole  -e ./inspection_console &
sleep 1
konsole  -e ./motor_x &
sleep 1
konsole  -e ./motor_z &
sleep 1 
konsole -e ./watchdog 