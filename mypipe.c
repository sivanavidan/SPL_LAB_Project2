#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int pipefd[2]; 

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        exit(1);
    }

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork(); 

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {

        close(pipefd[1]); 

        char buffer[1024];
        ssize_t count = read(pipefd[0], buffer, sizeof(buffer) - 1); 

        if (count < 0) {
            perror("read");
            exit(1);
        }

        buffer[count] = '\0'; 

        printf("Child received message: %s\n", buffer);

        close(pipefd[0]); 
    } else {

        close(pipefd[0]); 

        write(pipefd[1], argv[1], strlen(argv[1]));

        close(pipefd[1]); 

        wait(NULL);
    }

    return 0;
}
