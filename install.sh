unzip sources.zip -d $1
mkdir executables
gcc $1/sources/process_1/command_console.c -o executables/command_console &
gcc $1/sources/process_2/inspection_console.c -o executables/inspection_console &
gcc $1/sources/process_3/motor_x.c -o executables/motor_x &
gcc $1/sources/process_4/motor_z.c -o executables/motor_z &
gcc $1/sources/process_5/watchdog.c -o executables/watchdog &