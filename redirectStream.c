#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

void redirectStream(const char *inp, const char *cmd, const char *out)
{
    int input_fd, output_fd;

    // The child process is being forked
    pid_t pid = fork();

    if (pid == 0)
    {   // This is for the Child process
        // Redirecting the input
        if (strcmp(inp, "-") != 0)
        {
            input_fd = open(inp, O_RDONLY);
            if (input_fd < 0)
            {
                perror("Failed to open input file");
                exit(EXIT_FAILURE);
            }
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
            fprintf(stderr, "Input redirection set up successfully.\n");
        }

        // Redirecting the output
        if (strcmp(out, "-") != 0)
        {
            output_fd = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_fd < 0)
            {
                perror("Failed to open output file");
                exit(EXIT_FAILURE);
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
            fprintf(stderr, "Output redirection set up successfully.\n");
        }

        fprintf(stderr, "Executing command: %s\n", cmd);
        char *args[] = {"/bin/sh", "-c", (char *)cmd, NULL};
        execvp("/bin/sh", args);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // This is for the Parent process
        int status;
        waitpid(pid, &status, 0);
        fprintf(stderr, "Command executed, child process finished.\n");
    }
    else
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}

int main()
{
    redirectStream("input.txt", "grep 'hello'", "output.txt");
    return 0;
}
