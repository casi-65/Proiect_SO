#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

pid_t monitor_pid = -1;
int monitor_closing = 0;

void readPipe(int pipefd[2])
{
    char buffer[1024];
    ssize_t nbytes = read(pipefd[0], buffer, sizeof(buffer) - 1);
    if (nbytes > 0)
    {
        buffer[nbytes] = '\0';
        printf("Result from Treasure Manager:\n%s", buffer);
    }
    else
    {
        printf("Eroare la citire din pipe\n");
        exit(-1);
    }
}

void startMonitor(int pipefd[2])
{
    // Fork the process to create a new child process
    // that will run the monitor program
    if (pipe(pipefd) == -1)
    {
        printf("Error at pipe\n");
        exit(-1);
    }
    monitor_pid = fork();
    if (monitor_pid < 0)
    {
        printf("Fork error\n");
        exit(-1);
    }
    // Child process
    if (monitor_pid == 0)
    {
        close(pipefd[0]);
        // Replace the child process with the monitor program
        char fd_str[10];
        sprintf(fd_str, "%d", pipefd[1]);
        execl("./monitor", "monitor", fd_str, NULL);
        printf("execl failed\n");
        exit(-1);
    }
    // Parent process
    close(pipefd[1]);
    printf("Monitor started with PID %d\n", monitor_pid);
}

void handle_sigchld(int sig)
{
    int status;
    if (monitor_pid > 0)
    {
        pid_t pid = waitpid(monitor_pid, &status, WNOHANG);
        if (pid == monitor_pid)
        {
            printf("Monitor process has exited.\n>>> ");
            fflush(stdout);
            monitor_pid = -1;
            monitor_closing = 0;
        }
    }
}

int main(void)
{
    char command[21];
    // Set up the signal handler for SIGCHLD
    // This will allow us to handle the termination of the monitor process
    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);
    int pipefd[2];
    while (1)
    {
        // Print the prompt and wait for user input
        usleep(300000);
        if (!monitor_closing)
        {
            printf(">>> ");
        }
        fflush(stdout);
        // Read the command from the user
        if (!fgets(command, sizeof(command), stdin))
        {
            break;
        }
        // Remove the newline character from the command
        command[strcspn(command, "\n")] = '\0';
        if (monitor_closing)
        {
            printf("Monitor is shutting down, please wait...\n");
            continue;
        }

        if (strcmp(command, "start_monitor") == 0)
        {
            if (monitor_pid != -1)
            {
                printf("Monitor already running with PID %d\n", monitor_pid);
            }
            else
            {
                // Start the monitor process
                startMonitor(pipefd);
            }
        }
        else if (strcmp(command, "list_treasures") == 0)
        {
            if (monitor_pid == -1)
            {
                printf("Monitor not started.\n");
            }
            else
            {
                char hunt_id[100];
                printf("Enter hunt ID: ");
                fflush(stdout);
                fgets(hunt_id, sizeof(hunt_id), stdin);
                hunt_id[strcspn(hunt_id, "\n")] = '\0';

                FILE *f = fopen("prm.txt", "w");
                if (!f)
                {
                    printf("Failed to open prm.txt\n");
                    exit(-1);
                }

                fprintf(f, "--list\n%s\n", hunt_id);
                fclose(f);
                // Send SIGUSR1 to the monitor process
                if (kill(monitor_pid, SIGUSR1) == -1)
                {
                    printf("Failed to send SIGUSR1 to monitor\n");
                    // Set monitor_pid to -1 to indicate it's no longer valid
                    monitor_pid = -1;
                }
                else
                {
                    readPipe(pipefd);
                }
            }
        }
        else if (strcmp(command, "view_treasure") == 0)
        {
            if (monitor_pid == -1)
            {
                printf("Monitor not started.\n");
            }
            else
            {
                char hunt_id[100], treasure_id[100];
                printf("Enter hunt ID: ");
                fflush(stdout);
                fgets(hunt_id, sizeof(hunt_id), stdin);
                hunt_id[strcspn(hunt_id, "\n")] = '\0';

                printf("Enter treasure ID: ");
                fflush(stdout);
                fgets(treasure_id, sizeof(treasure_id), stdin);
                treasure_id[strcspn(treasure_id, "\n")] = '\0';

                FILE *f = fopen("prm.txt", "w");
                if (!f)
                {
                    printf("Failed to open prm.txt");
                    exit(-1);
                }

                fprintf(f, "--view\n%s\n%s\n", hunt_id, treasure_id);
                fclose(f);
                // Send SIGUSR1 to the monitor process
                if (kill(monitor_pid, SIGUSR1) == -1)
                {
                    printf("Failed to send SIGUSR1 to monitor\n");
                    // Set monitor_pid to -1 to indicate it's no longer valid
                    monitor_pid = -1;
                }
                else
                {
                    readPipe(pipefd);
                }
            }
        }
        else if (strcmp(command, "list_hunts") == 0)
        {
            if (monitor_pid == -1)
            {
                printf("Monitor not started.\n");
            }
            else
            {
                FILE *f = fopen("prm.txt", "w");
                if (!f)
                {
                    printf("Failed to open prm.txt");
                    exit(-1);
                }

                fprintf(f, "--list_hunts\n");
                fclose(f);
                // Send SIGUSR1 to the monitor process
                if (kill(monitor_pid, SIGUSR1) == -1)
                {
                    printf("Failed to send SIGUSR1 to monitor\n");
                    // Set monitor_pid to -1 to indicate it's no longer valid
                    monitor_pid = -1;
                }
                else
                {
                    readPipe(pipefd);
                }
            }
        }
        else if (strcmp(command, "stop_monitor") == 0)
        {
            if (monitor_pid == -1)
            {
                printf("Monitor not started.\n");
            }
            else
            {
                monitor_closing = 1; // Set the flag to indicate monitor is closing
                if (kill(monitor_pid, SIGTERM) == -1)
                {
                    printf("Failed to send SIGTERM\n");
                    monitor_closing = 0;
                }
                else
                {
                    close(pipefd[0]); // Close the read end of the pipe
                    printf("Waiting for monitor to shut down...\n");
                }
            }
        }
        else if (strcmp(command, "exit") == 0)
        {
            if (monitor_pid != -1)
            {
                printf("Monitor still running with PID %d, you need to stop it first.\n", monitor_pid);
            }
            else
            {
                // Exit the program
                break;
            }
        }
        else if (strcmp(command, "calculate_score") == 0)
        {
            DIR *dir = opendir(".");
            struct dirent *entry; // Directory entry

            if (!dir)
            {
                printf("Error at opening directory\n");
                continue;
            }
            // Loop through the directory entries
            while ((entry = readdir(dir)) != NULL)
            {
                struct stat st;
                // Verify if the entry is a directory
                if (stat(entry->d_name, &st) == 0 && S_ISDIR(st.st_mode))
                {
                    // Build the path to the treasure file
                    char treasure_path[256];
                    snprintf(treasure_path, sizeof(treasure_path), "%s/treasure.dat", entry->d_name);
                    // Check if the treasure file exists
                    if (access(treasure_path, F_OK) == 0) 
                    {
                        int pipefd2[2]; // File descriptor for the pipe
                        if (pipe(pipefd2) == -1)
                        {
                            printf("Pipe Error\n");
                            continue;
                        }

                        pid_t pid = fork();
                        if (pid == 0)
                        {
                            close(pipefd2[0]); // Close the read end of the pipe
                            dup2(pipefd2[1], STDOUT_FILENO); // Redirect stdout to the pipe
                            close(pipefd2[1]); // Close the write end of the pipe
                            execl("./calculate_score", "calculate_score", entry->d_name, NULL);
                            printf("Error execl\n");
                            exit(-1);
                        }
                        else if (pid > 0)
                        {
                            close(pipefd2[1]); // Close the write end of the pipe
                            char buffer[1024];
                            ssize_t bytes;
                            // Read the output from the child process
                            while ((bytes = read(pipefd2[0], buffer, sizeof(buffer) - 1)) > 0)
                            {
                                buffer[bytes] = '\0';
                                printf("%s", buffer);
                            }
                            close(pipefd2[0]); // Close the read end of the pipe
                            wait(NULL); // Wait for the child process to finish
                        }
                        else
                        {
                            printf("Error fork\n");
                        }
                    }
                }
            }

            closedir(dir); // Close the directory
        }
        else if (strcmp(command, "help") == 0)
        {
            printf("Available commands:\n");
            printf("  start_monitor     - Start the monitor process\n");
            printf("  list_hunts        - List all available hunts\n");
            printf("  list_treasures    - List treasures in a hunt\n");
            printf("  view_treasure     - View a specific treasure\n");
            printf("  stop_monitor      - Stop the monitor process\n");
            printf("  exit              - Exit this program (if monitor is stopped)\n");
            printf("  calculate_score   - Calculate and display score for each user in all hunts\n");
        }
        else
        {
            printf("Unknown command. Type 'help' for list of commands.\n");
        }
    }
    return 0;
}
