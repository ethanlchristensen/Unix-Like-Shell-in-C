# Unix-Like Shell in C

This program mimics a Unix-like shell. It is created with C and is meant to run on Unix systems. A user can execute external Unix commands such as ls, calendar, nano, etc...

Some chunks of code are commented out. These were meant to act as debuguing agents. You can uncomment them to get a better understanding on how the program processes incoming commands.

To exit out of the shell, you must `ctrl + c`

## Installation

Use the c-compiler to compile the shell.c
```bash
gcc shell.c -o shell
```
Run the resulting file
```bash
./shell
```
## Usage
The program can run external Unix commands that are installed on the system. Here are some examples

```bash
>> ls

>> mkdir test_folder

>> ls
* now test_folder should appear *

>> rm -r test_folder

>> ls -l -F -h

>> false && echo Hello && false || cal

```
## Snapshots

```bash
***********************************************************
* This is a simple shell made by Ethan Christensen for    *
* CSCI4500 at the Univerity of Nebrasaka at Omaha. This   *
* shell aims to emulate the a UNIX-type shell but only    *
* for external commands, and not commands that are built  *
* into the system. This is meant to be a simple shell and *
* is not meant to emulate every command on a UNIX-type    *
* shell.                                                  *
*                                                         *
* Author: Ethan Christensen                               *
* Resources: Professor Jose Santos                        *
***********************************************************
>> ls

hello  hello.c  shell  shell.c

>> mkdir test_folder


>> ls

hello  hello.c  shell  shell.c  test_folder

>> cd test_folder

Error: path: "cd" not found.

>> rm -r test_folder


>> ls

hello  hello.c  shell  shell.c

>> ls -l -F -h

total 60K
-rwxr-xr-x 1 ethanchristensen domain users 17K Dec 12  2021 hello*
-rw-r--r-- 1 ethanchristensen domain users 118 Dec 12  2021 hello.c
-rwxr-xr-x 1 ethanchristensen domain users 18K Dec  9  2021 shell*
-rw-r--r-- 1 ethanchristensen domain users 16K Dec  9  2021 shell.c

>> false && echo Hello && false || cal

   September 2022
Su Mo Tu We Th Fr Sa
             1  2  3
 4  5  6  7  8  9 10
11 12 13 14 15 16 17
18 19 20 21 22 23 24
25 26 27 28 29 30

```

## Have Fun :)
