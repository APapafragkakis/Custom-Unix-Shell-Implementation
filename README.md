# C Shell Implementation - hy345sh

## Description
Assignment 1 - Operating Systems (HY345)
Author: Alexandros Papafragakis

In this assignment, i will implement a C shell (hy345sh) to familiarize yourself with its operation. Specifically, you will learn how to create and control processes, understand pipes, and implement input-output redirection. Additionally, the shell will support basic commands and functionalities.

## Implementation

### Simple Commands
The shell should be capable of reading and executing commands entered by the user. For each command, the shell will create a child process to execute the user's command. It should support multiple commands separated by ";" and commands with arguments. You can use commands like ls, date, cat y1; rm y1, mkdir test, etc., to test your shell's functionality.

### Additional Commands
In addition to basic commands, the shell should support the following commands:
- `quit`: Terminate the execution of the shell.
- `chdir`: Change the current working directory.

### Pipes
The shell should support pipes using the "|" symbol, allowing the direct redirection of standard output from one command to the standard input of another.

### Redirection
The shell should support input and output redirection:
- Input redirection using the "<" symbol: Contents of the specified file are passed as input to the command (e.g., cat < test.txt).
- Output redirection using the ">" symbol: Standard output of the command is redirected to the specified file. Previous content of the file is overwritten (e.g., ls -al > x.txt).
- Append output redirection using the ">>" symbol: Standard output of the command is appended to the specified file without overwriting previous content (e.g., ls >> y.txt).

## Usage
Compile the C shell using a C compiler, and run the compiled executable.

```bash
gcc -o hy345sh hy345sh.c
./hy345sh
