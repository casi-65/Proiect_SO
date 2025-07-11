#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>


int pipe_fd[2];// File descriptor for the pipe

void handle_SIGTERM(int sig)
{
    printf("Shutting down after delay...\n");
    fflush(stdout);
    usleep(3000000);
    printf("Terminated.\n");
    // Close the pipe
    close(pipe_fd[1]);
    exit(0); // Terminate the process
}

void handle_SIGUSR1(int sig)
{
    // Open the file to read the command and arguments
    FILE *f = fopen("prm.txt", "r");
    if (!f)
    {
        printf("Could not open prm.txt\n");
        exit(-1);
    }
    char args[3][20];
    int count = 0;
    // Read the command and arguments from the file
    while (fgets(args[count], sizeof(args[count]), f) && count < 3)
    {
        // Remove the newline character from the string
        args[count][strcspn(args[count], "\n")] = 0;
        count++;
    }
    fclose(f);
    if (unlink("prm.txt") == -1)
    {
        printf("Failed to remove prm.txt\n");
    }
    if (count == 0)
    {
        printf("prm.txt is empty\n");
        return;
    }
    // Prepare the arguments for execvp
    char *exec_args[5] = {"./treasure_manager", NULL, NULL, NULL};
    for (int i = 0; i < count; i++)
    {
        exec_args[i + 1] = args[i]; // Start from index 1 to leave space for the program name
    }
    pid_t pid = fork();
    if (pid == 0)
    {
        // Child process
        // Execute the command
        dup2(pipe_fd[1], STDOUT_FILENO);
        // Close the write end of the pipe
        close(pipe_fd[1]);
        execvp(exec_args[0], exec_args);
        printf("execvp failed\n");
        exit(1);
    }
    else if (pid > 0)
    {
        int status;
        // Parent process
        // Wait for the child process to finish
        waitpid(pid, &status, 0);
        fflush(stdout);
    }
    else
    {
        printf("fork failed\n");
        exit(-1);
    }
}

int main(int argc, char **argv)
{
    pipe_fd[1] = atoi(argv[1]); // Get the write end of the pipe from command line argument
    // Set up the signal handler for SIGUSR1 and SIGTERM
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_SIGUSR1;
    sigaction(SIGUSR1, &sa, NULL);
    sa.sa_handler = handle_SIGTERM;
    sigaction(SIGTERM, &sa, NULL);
    while (1)
        pause();
    return 0;
}