#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void startMonitor(void)
{
    int monitor_pid;
    if((monitor_pid=fork())<0)
    {
        printf("Fork Error\n");
        exit(-1);
    }
    if(monitor_pid==0)
    {
        //code for child
    }

}

int main(int argc,char **argv)
{
    if(argc <= 1)
    {
        fprintf(stderr, "Usage:\n"
            "start_monitor\n"
            "list_hunts\n"
            "list_treasures\n"
            "view_treasure\n"
            "stop_monitor\n"
            "exit\n");
        exit(-1);
    }
    if(strcmp(argv[1],"start_monitor")==0)
    {
        startMonitor();
    }
    else if(strcmp(argv[1],"list_hunts")==0)
    {

    }
    else if(strcmp(argv[1],"list_treasures")==0)
    {
        
    }
    else if(strcmp(argv[1],"view_treasure")==0)
    {
        
    }
    else if(strcmp(argv[1],"stop_monitor")==0)
    {
        
    }
    else if(strcmp(argv[1],"exit")==0)
    {
        
    }
    else 
    {
        printf("Invalid command\n");
        exit(-1);
    }
    return 0;

}