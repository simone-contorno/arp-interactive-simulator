gcc command_console.c -o command_console &
gcc inspection_console.c -o inspection_console &
gcc motor_x.c -o motor_x &
gcc motor_z.c -o motor_z &
gcc watchdog.c -o watchdog &

konsole -p LocalTabTitleFormat='Command console' -e ./command_console &
sleep 1
konsole -p LocalTabTitleFormat='Inspection console' -e ./inspection_console &
sleep 1
konsole -p LocalTabTitleFormat='Motor X' -e ./motor_x &
sleep 1
konsole -p LocalTabTitleFormat='Motor Z' -e ./motor_z &
sleep 1 
konsole -p LocalTabTitleFormat='Watchdog' -e ./watchdog 