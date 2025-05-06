#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t monitor_pid = -1;

void startMonitor(void)
{
    // Fork the process to create a new child process
    // that will run the monitor program
    monitor_pid = fork();
    if (monitor_pid < 0)
    {
        printf("Fork error\n");
        exit(-1);
    }
    // Child process
    if (monitor_pid == 0)
    {
        // Replace the child process with the monitor program
        execl("./monitor", "monitor", NULL);
        printf("execl failed\n");
        exit(-1);
    }
    // Parent process
    printf("Monitor started with PID %d\n", monitor_pid);
}

void handle_sigchld(int sig)
{
    int status;
    // Wait for any child process to change state
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (pid == monitor_pid)
    {
        printf("Monitor process has exited.\n");
        monitor_pid = -1;
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

    while (1)
    {
        // Print the prompt and wait for user input
        usleep(300000);
        printf(">>> ");
        fflush(stdout);
        // Read the command from the user
        if (!fgets(command, sizeof(command), stdin))
        {
            break;
        }
        // Remove the newline character from the command
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "start_monitor") == 0)
        {
            if (monitor_pid != -1)
            {
                printf("Monitor already running with PID %d\n", monitor_pid);
            }
            else
            {
                // Start the monitor process
                startMonitor();
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
                    monitor_pid = -1; // presupunem că nu mai este valid
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
                    monitor_pid = -1; // presupunem că nu mai este valid
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
                // Send SIGTERM to the monitor process
                kill(monitor_pid, SIGTERM);
                printf("Waiting for monitor to shut down...\n");
                // Wait for the monitor process to terminate
                while (monitor_pid != -1)
                {
                    // Check if the monitor process has terminated
                    usleep(300000);
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
        else if (strcmp(command, "help") == 0)
        {
            printf("Available commands:\n");
            printf("  start_monitor     - Start the monitor process\n");
            printf("  list_hunts        - List all available hunts\n");
            printf("  list_treasures    - List treasures in a hunt\n");
            printf("  view_treasure     - View a specific treasure\n");
            printf("  stop_monitor      - Stop the monitor process\n");
            printf("  exit              - Exit this program (if monitor is stopped)\n");
        }
        else
        {
            printf("Unknown command. Type 'help' for list of commands.\n");
        }
    }

    return 0;
}