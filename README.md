# hy345sh - Custom Shell Implementation

**Course:** Operating Systems (HY345)  
**Assignment:** Exercise 1  
**Language:** C

## Overview

This project implements a custom Unix shell called `hy345sh` that supports basic shell operations including command execution, piping, I/O redirection, and environment variables.

## Features

### 1. Shell Prompt
The shell displays a custom prompt in the format:
```
<Student_ID>-hy345sh@<Username>:<Directory>
```
Where:
- `<Student_ID>`: Your student registration number (e.g., csd5072)
- `<Username>`: Current logged-in user
- `<Directory>`: Current working directory

### 2. Simple Commands
- Execute single commands with arguments
- Support multiple commands separated by semicolons (`;`)
- Create child processes for each command execution
- Examples:
  ```bash
  ls
  date
  cat file.txt; rm file.txt
  mkdir test
  ```

### 3. Environment Variables
- Define and use global environment variables
- Set variables: `x=world`
- Use variables: `echo hello $x` → outputs "hello world"

### 4. Pipes
- Chain commands using the pipe operator (`|`)
- Redirect stdout of one command to stdin of another
- Example: `ls | sort | uniq | wc`

### 5. I/O Redirection

#### Input Redirection (`<`)
```bash
cat < test.txt
```

#### Output Redirection (`>`)
```bash
ls -al > output.txt    # Overwrites file
```

#### Append Redirection (`>>`)
```bash
ls >> output.txt       # Appends to file
```

## Compilation

A Makefile is provided for easy compilation:

```bash
make all    # Compile the shell
make clean  # Remove generated files
```

## Testing Commands

Test the shell with these complex command combinations:

```bash
# Basic redirection
ls -al > y1
cat y1; rm y1

# Variables
age=20; echo I am $age years old

# Complex piping
ls | sort | uniq | wc

# Combined operations
echo hello > yy; echo world >> yy; cat < yy | sort | uniq | wc > y1; cat y1

# Variable concatenation
func="5+5="; res=10; echo $func$res

# Advanced examples
my_var="This is my var"; echo $my_var | wc > xx; cat < xx
x="5|5;10"; echo $x > file; cat < file
```

## Implementation Details

### System Calls Used
- `fork(2)` - Process creation
- `exec(3)` - Program execution
- `pipe(2)` - Inter-process communication
- `dup2(2)` - File descriptor duplication
- `setenv(3)` - Environment variable setting
- `getenv(3)` - Environment variable retrieval

### Key Components
1. **Command Parser**: Tokenizes input and handles special characters
2. **Process Manager**: Creates and manages child processes
3. **Pipe Handler**: Implements piping between commands
4. **Redirection Handler**: Manages file I/O redirection
5. **Variable Manager**: Handles environment variable operations

## Running in QEMU

This shell is designed to run in the QEMU emulator:

1. Copy the virtual disk image:
   ```bash
   cp ~hy345/qemu-linux/hy345-development.img .
   ```

2. Start QEMU:
   ```bash
   qemu-system-i386 -hda hy345-development.img -curses
   ```

3. Login credentials: username `hy345`, password `hy345`

4. Transfer files using SCP:
   ```bash
   scp username@10.0.2.2:~/shell.c .
   ```

5. Save work frequently:
   ```bash
   save
   ```

6. Exit QEMU safely:
   ```bash
   turnoff
   ```

## File Structure

```
assignment_1/
├── shell.c          # Main shell implementation
├── Makefile         # Build configuration
├── README.md        # This file
└── test_commands.sh # Test script (optional)
```

## Important Notes

- **Individual Work**: This is strictly an individual assignment
- **Code Quality**: Keep code clean and well-commented
- **Error Handling**: Implement robust error handling for all operations
- **Memory Management**: Properly manage memory allocation and deallocation
- **Signal Handling**: Handle signals appropriately

## Submission

Submit your work using:
```bash
turnin assignment_1@hy345 <directory_name>
```

## Author Information


## Resources

- [fork(2) manual](https://man7.org/linux/man-pages/man2/fork.2.html)
- [exec(3) manual](https://man7.org/linux/man-pages/man3/exec.3.html)
- [pipe(2) manual](https://man7.org/linux/man-pages/man2/pipe.2.html)
- [dup2(2) manual](https://man7.org/linux/man-pages/man2/dup.2.html)
- [setenv(3) manual](https://www.man7.org/linux/man-pages/man3/setenv.3.html)
- [getenv(3) manual](https://man7.org/linux/man-pages/man3/getenv.3.html)

