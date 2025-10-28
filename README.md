# Shell and Pipe Implementation

## Overview

Lab 2 implements a simplified UNIX shell and a pipe communication program to demonstrate process creation, management, and inter-process communication in Linux.
The shell reads user commands, interprets them, executes programs, manages background processes, handles signals, and supports input/output redirection.
The pipe program shows how to transfer data between parent and child processes using the `pipe()` system call.

## Components

1. **myshell.c** – A basic command interpreter supporting:

   * Running commands using `fork` and `execvp`
   * Built-in commands: `cd`, `quit`, `halt`, `wakeup`, `ice`
   * Background execution (`&`) and redirection (`<`, `>`)
   * Signal handling (SIGINT, SIGTSTP, SIGCONT)

2. **mypipe.c** – Demonstrates `pipe()` by sending a message from a parent process to its child.

## Build and Run

Compile both programs using the makefile:

```
make myshell
make mypipe
```

Run the shell:

```
./myshell
```

Run the pipe example:

```
./mypipe "Hello world"
```


