#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t monitor_pid = -1;

void startMonitor(void)
{
    monitor_pid = fork(); // Creăm un nou proces

    if (monitor_pid < 0)
    {
        printf("Fork error\n");
        exit(-1);
    }

    if (monitor_pid == 0)
    {
        execl("./treasure_manager", "treasure_manager", NULL);
        printf("execl failed\n");
        exit(-1);
    }

    // Suntem în procesul părinte — copilul rulează treasure_manager
    printf("Monitor started with PID %d\n", monitor_pid);
}

int main(void)
{
    char command[21];
    while (1)
    {
        printf(">>> ");
        fflush(stdout);
        fgets(command, sizeof(command), stdin);
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
                printf("Monitor started.\n");
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
                FILE *f = fopen("params.txt", "w");
                if (!f)
                {
                    printf("Failed to open params.txt\n");
                    continue;
                }
                fprintf(f, "%s\n", hunt_id);
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
                FILE *f = fopen("params.txt", "w");
                if (!f)
                {
                    perror("Failed to open params.txt\n");
                    continue;
                }
                fprintf(f, "%s\n%s\n", hunt_id, treasure_id);
                fclose(f);
                kill(monitor_pid, SIGUSR2);
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

                int status;
                waitpid(monitor_pid, &status, 0);
                printf("Monitor stopped.\n");
                monitor_pid = -1;
            }
        }
        else if (strcmp(command, "exit") == 0)
        {
            if (monitor_pid != -1)
            {
                printf("Monitor still running with PID %d, you need to stop it\n", monitor_pid);
            }
            else
            {
                break;
            }
        }
    }
    return 0;
}