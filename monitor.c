#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void handle_SIGTERM(int sig)
{
    printf("Shutting down after delay...\n");
    fflush(stdout);
    usleep(3000000);
    printf("Terminated.\n");
    exit(0);
}

void handle_SIGUSR1(int sig)
{
    FILE *f = fopen("prm.txt", "r");
    if (!f)
    {
        printf("Could not open prm.txt\n");
        exit(-1);
    }
    char args[3][20];
    int count = 0;
    while (fgets(args[count], sizeof(args[count]), f) && count < 3)
    {
        args[count][strcspn(args[count], "\n")] = 0;
        count++;
    }
    fclose(f);
    if (count == 0)
    {
        printf("prm.txt is empty\n");
        return;
    }
    char *exec_args[5] = {"./treasure_manager", NULL, NULL, NULL};
    for (int i = 0; i < count; i++)
    {
        exec_args[i + 1] = args[i];
    }
    pid_t pid = fork();
    if (pid == 0)
    {
        execvp(exec_args[0], exec_args);
        printf("execvp failed");
        exit(1);
    }
    else if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
        fflush(stdout);
    }
    else
    {
        printf("fork failed\n");
        exit(-1);
    }
}

int main(void)
{
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_SIGUSR1;
    sigaction(SIGUSR1, &sa, NULL);
    sa.sa_handler = handle_SIGTERM;
    sigaction(SIGTERM, &sa, NULL);
    // printf("Waiting for command: \n"
    //        "1): list_hunts \n"
    //        "2): list_treasures\n"
    //        "3): view_treasure\n"
    //        "4): stop_monitor\n");
    while (1)
        pause();
    return 0;
}