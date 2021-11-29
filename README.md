# Advanced and Robot Programming
## Interactive simulator of hoist with 2 d.o.f.
### Robotics Engineering - Simone Contorno

<br>

### Introduction
An overview of this program function.<br>
[Go to Introduction](#intro)

### How it works
A rapid description of how the program works.<br>
[Go to How it works](#how)

### Installation and Execution
How install and run this program in Linux.<br>
[Go to Installation and Execution](#installation)

### Conclusion
Conclusion.<br>
[Go to Conclusion](#con)

<a name="intro"></a>
### Introduction

This program test and deploy is an interactive simulator of hoist with 2 d.o.f., 
in which two different consoles allow the user to activate the hoist.<br>
In the octagonal box there are two motors motor_x and motor_z, 
which displace the hoist along the two respective axes.<br>
Motions along axes have their bounds.<br>
From the user side there are two consoles (shell windows) and keys with different aims, 
that simulate a real system.

<a name="how"></a>
### How it works

There are 5 processes that work as follow:
<ol>
    <li><b>command console</b>: reads 6 commands typed in the shell window from the user; 
    the commands are:
    <ol>
        <li>X axis increase;</li>
        <li>X axis decrease;</li>
        <li>X axis stop;</li>
        <li>Z axis increase;</li>
        <li>Z axis decrease;</li>
        <li>Z axis stop.</li>
    </ol>
    </li>
    <li><b>inspection console</b>: receives from motors the hoist positions while it moves, 
    and reports them on the screen; reads 6 commands typed in the shell window from the user:
    <ol>
        <li>S: emergency stop, the joist stops immediately until a command from the first console arrives;</li>
        <li>R: reset, the joint stops, both axes go to a zero position and wait for commands.</li>
    </ol>
    </li>
    <li><b>motor x</b>: simulates the motion along x axis, receives command and sends back the real time position.</li>
    <li><b>motor z</b>: simulates the motion along z axis, receives command and sends back the real time position.</li>
    <li><b>watchdog</b>: checks the previous 4 processes periodically, and sends a reset (like the R command) in
    case all processes did nothing (no computation, no motion, no input/output) for a certain time.</li>
</ol>

The motor process simulates the constant speed motion, including a simple error in position
measurement. They receives a command (move, stop, go to zero position), acts, sends back a position
estimate, stops when the end displacement is reached. 

<a name="installation"></a>
### Installation and Execution

Download the repository:

<pre>
<code>
git clone https://github.com/simone-contorno/ARP-Assignment-1
</code>
</pre>

After, go into the directory and make executable all .sh files:

<pre>
<code>
chmod +x install.sh run.sh help.sh
</code>
</pre>

To compile the source files, type:

<pre>
<code>
./install.sh
</code>
</pre>

Now, to exec the program, type:

<pre>
<code>
./run.sh 
</code>
</pre>

If you want read some specific description about a process, type:

<pre>
<code>
./help.sh <number>
</code>
</pre>

where <number> is the corresponding process text file description.

<a name="con"></a>
### Conclusion

Thanks to have read this file, i hope it was clear and interesting.<br>
<br>
Collaborator: Gabriele Russo - @GabrieleRusso11