konsole -p LocalTabTitleFormat='Command console' -e ./executables/command_console &
sleep 1
konsole -p LocalTabTitleFormat='Inspection console' -e ./executables/inspection_console &
sleep 1
konsole -p LocalTabTitleFormat='Motor X' -e ./executables/motor_x &
sleep 1
konsole -p LocalTabTitleFormat='Motor Z' -e ./executables/motor_z &
sleep 1 
konsole -p LocalTabTitleFormat='Watchdog' -e ./executables/watchdog 