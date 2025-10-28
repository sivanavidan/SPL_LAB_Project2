#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int pipefd[2]; // pipefd[0] לקריאה, pipefd[1] לכתיבה

    // בדיקה שהמשתמש סיפק טקסט לשליחה
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        exit(1);
    }

    // יצירת הצינור
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork(); // יצירת תהליך ילד

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // תהליך ילד

        close(pipefd[1]); // סוגר את צד הכתיבה - הילד רק קורא

        char buffer[1024];
        ssize_t count = read(pipefd[0], buffer, sizeof(buffer) - 1); // קורא מהצינור

        if (count < 0) {
            perror("read");
            exit(1);
        }

        buffer[count] = '\0'; // סוגר את המחרוזת

        printf("Child received message: %s\n", buffer);

        close(pipefd[0]); // סוגר את צד הקריאה
    } else {
        // תהליך הורה

        close(pipefd[0]); // סוגר את צד הקריאה - ההורה רק כותב

        // שולח את ההודעה לילד
        write(pipefd[1], argv[1], strlen(argv[1]));

        close(pipefd[1]); // סוגר את צד הכתיבה

        wait(NULL); // ממתין לסיום הילד
    }

    return 0;
}
