MouseK-driver
====

A linux kernel module to control mouse pointer using keyboard.
This is an experiment to fiddle with drivers. It may not have any real life use case but was fun to create.
It still has some bugs and needs a lot of improvements.

### To run

+ Use `make` to generate .ko file
+ Install using `insmod mousek.ko files`
+ Go to `cd /dev` and create `sudo mknod /dev/mousek c 247 0` where 247 will be replaced by major number given while installing this module. This major number can be viewed in logs using `dmesg`
+ Provide permission to this char file `sudo chmod 666 mousek`
+ To remove `rmmod mousek`
+ To view available kernel mods `lsmod`

### To provide input to mouse

Use the available commands:
All of the commands are being fed into dev mousek char file. This can be done by echo in `/dev` directory.
For example -
```sh
$ echo "i dllwW" > mousek
$ echo "ll" > mousek
$ echo "x 434" > mousek
```
First character will tell what command to follow
i : instruction sequence followed by commands
example 
+ `echo "i udrrl" > mousek` or `echo "i ddrqQ" > mousek` where -
  + "i" is command
  + "l" for -10 pixel X axis
  + "r" for 10 pixel X axis
  + "d" for 10 pixel Y axis
  + "u" for -10 pixel Y axis
  + "q" for left click down
  + "Q" for left click Up
  + "w" for right click down
  + "W" for right click Up


+ x : relative value in X Axis
  + Example: `echo "x 250" > mousek`

+ y : relative value in Y Axis
  + Example: `echo "y 200" > mousek`

+ l : left click , ll : double left click
  + Example: `echo "l" > mousek` or `echo "ll" > mousek`

+ r : right click
  + Example: `echo "r" > mousek`


### Bugs
+ Input sequence command doesn't respect order of commands at the moment.
+ Unable to move visible mouse pointer symbol on screen (for this, need to mess with x10 demon I guess).

### License
This kernel module comes with a GPL license.