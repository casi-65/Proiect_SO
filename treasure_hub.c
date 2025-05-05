#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t monitor_pid = -1;

void startMonitor(void) 
{
    monitor_pid = fork();

    if (monitor_pid < 0) 
    {
        printf("Fork error\n");
        exit(-1);
    }

    if (monitor_pid == 0) 
    {
        execl("./monitor", "monitor", NULL);
        printf("execl failed\n");
        exit(-1);
    }

    printf("Monitor started with PID %d\n", monitor_pid);
}


void handle_sigchld(int sig) 
{
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (pid == monitor_pid) 
    {
        printf("Monitor process has exited.\n");
        monitor_pid = -1;
    }
}

int main(void) {
    char command[21];

    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    while (1) 
    {
        usleep(10000);
        printf(">>> ");
        fflush(stdout);
        if (!fgets(command, sizeof(command), stdin)) 
        {
            break;
        }

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "start_monitor") == 0) 
        {
            if (monitor_pid != -1) 
            {
                printf("Monitor already running with PID %d\n", monitor_pid);
            } 
            else 
            {
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
                kill(monitor_pid, SIGUSR1);
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
                kill(monitor_pid, SIGUSR1);
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
                kill(monitor_pid, SIGTERM);
                printf("Waiting for monitor to shut down...\n");
                // Așteaptă ca handlerul SIGCHLD să seteze monitor_pid = -1
                while (monitor_pid != -1) 
                {
                    usleep(100000);
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
                break;
            }

        } 
        else if (strcmp(command, "help") == 0) 
        {
            printf("Available commands:\n");
            printf("  start_monitor     - Start the monitor process\n");
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