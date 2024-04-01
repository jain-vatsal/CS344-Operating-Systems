// question 3

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

int main() {
    char input[256];
    int c=1;
    while (c!=0) {
        printf("Enter a command (e.g., './compute_sequence_1 | ./compute_sequence_2') or 'quit' to exit: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0'; // Remove newline character

        if (strcmp(input, "quit") == 0)
            {
                printf("yes exit\n\n");
                break;
            }

        // Create a pipe
        int fd[2];
        if (pipe(fd) == -1) {
            perror("Pipe creation failed");
            exit(EXIT_FAILURE);
        }

        // Fork a child process
        pid_t child_pid = fork();

        if (child_pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (child_pid == 0) { // Child process
            close(fd[1]); // Close the write end of the pipe
            dup2(fd[0], STDIN_FILENO); // Redirect stdin to the read end of the pipe
            close(fd[0]); // Close the read end of the pipe

            // Execute the second program
            execlp("./compute_sequence_2", "./compute_sequence_2", NULL);
            perror("Exec failed");
            exit(EXIT_FAILURE);
        } else { // Parent process
            close(fd[0]); // Close the read end of the pipe
            dup2(fd[1], STDOUT_FILENO); // Redirect stdout to the write end of the pipe
            close(fd[1]); // Close the write end of the pipe

            // Execute the first program
            execlp("./compute_sequence_1", "./compute_sequence_1", NULL);
            perror("Exec failed");
            // exit(EXIT_FAILURE);
        }

        printf("here it is now\n\n");
    }

    return 0;
}
