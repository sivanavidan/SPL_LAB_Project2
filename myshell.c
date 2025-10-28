#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <signal.h>
#include "LineParser.h"

#define MAX_LINE_LEN 2048

int debug_mode = 0;

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void handle_sigchld(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void execute(cmdLine *pCmdLine) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) { //child process

        if (pCmdLine->inputRedirect) {
            int inFile = open(pCmdLine->inputRedirect, O_RDONLY);
            if (inFile < 0) {
                perror("Input redirection failed");
                exit(1);
            }
            if (dup2(inFile, STDIN_FILENO) < 0) {
                perror("dup2 failed on input");
                close(inFile);
                exit(1);
            }
            close(inFile);
        }

        if (pCmdLine->outputRedirect) {
            int outFile = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (outFile < 0) {
                perror("Output redirection failed");
                exit(1);
            }
            if (dup2(outFile, STDOUT_FILENO) < 0) {
                perror("dup2 failed on output");
                close(outFile);
                exit(1);
            }
            close(outFile);
        }

        execvp(pCmdLine->arguments[0], pCmdLine->arguments);
        perror("Exec failed");
        exit(1);
    } else { 
        if (debug_mode)
            fprintf(stderr, "PID: %d, Executing command: %s\n", pid, pCmdLine->arguments[0]);

        if (pCmdLine->blocking)
            waitpid(pid, NULL, 0);
    }
    
}

int main(int argc, char **argv) {
    signal(SIGCHLD, handle_sigchld);

    char input[MAX_LINE_LEN];
    cmdLine *line;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0)
            debug_mode = 1;
    }

    while (1) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s> ", cwd);
            fflush(stdout);
        } else {
            perror("getcwd() error");
        }

        if (fgets(input, MAX_LINE_LEN, stdin) == NULL)
            break;

        line = parseCmdLines(input);
        if (!line)
            continue;

        if (strcmp(line->arguments[0], "quit") == 0) {
            freeCmdLines(line);
            break;
        }

        if (strcmp(line->arguments[0], "cd") == 0) {
            if (line->argCount > 1) {
                if (chdir(line->arguments[1]) != 0)
                    perror("cd failed");
            } else {
                fprintf(stderr, "cd requires an argument\n");
            }
        }

       
        else if (strcmp(line->arguments[0], "halt") == 0 ||
                 strcmp(line->arguments[0], "wakeup") == 0 ||
                 strcmp(line->arguments[0], "ice") == 0) {

            if (line->argCount < 2) {
                fprintf(stderr, "%s requires a process ID\n", line->arguments[0]);
            } else {
                int pid = atoi(line->arguments[1]);
                int sig;

                if (strcmp(line->arguments[0], "halt") == 0)
                    sig = SIGTSTP;
                else if (strcmp(line->arguments[0], "wakeup") == 0)
                    sig = SIGCONT;
                else
                    sig = SIGINT;

                if (kill(pid, sig) == -1)
                    perror("Failed to send signal");
            }
        }

            
        else {
            execute(line);
        }

        freeCmdLines(line);
    }

    return 0;
}
