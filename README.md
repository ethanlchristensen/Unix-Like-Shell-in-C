# Unix-Like Shell in C

This program mimics a Unix-like shell. It is created with C and is meant to run on Unix systems. A user can execute external Unix commands such as ls, calendar, nano, etc...

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

## Have Fun :)
