#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
    char TreasureID[50];
    char UserName[50];
    float Latitude;
    float Longitude;
    char ClueText[100];
    int Value;
}Treasure;

void add_treasure(const char *hunt_id)
{
    struct stat st = {0};
    if (stat(hunt_id, &st) == -1) {
        mkdir(hunt_id, 0700);
    }

    int f;
    char buffer[100];  
    strcpy(buffer, hunt_id);
    strcat(buffer, "/treasure.dat");
    // Check if the file exists and open it for appending and writing
    f = open(buffer, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (f < 0) {
        printf("Error opening file\n");
        exit(-1);
    }
    // Reading the structure from the user
    Treasure t;
    printf("Enter Treasure ID: ");
    if ((scanf("%s", t.TreasureID)) != 1) {
        printf("Error reading Treasure ID\n");
        exit(-1);
    }

    printf("Enter User Name: ");
    if ((scanf("%s", t.UserName)) != 1) {
        printf("Error reading User Name\n");
        exit(-1);
    }

    printf("Enter Latitude: ");
    if ((scanf("%f", &t.Latitude)) != 1) {
        printf("Error reading Latitude\n");
        exit(-1);
    }

    printf("Enter Longitude: ");
    if ((scanf("%f", &t.Longitude)) != 1) {
        printf("Error reading Longitude\n");
        exit(-1);
    }

    printf("Enter Clue Text: ");
    getchar();  // Clear newline left by previous scanf
    if ((fgets(t.ClueText, sizeof(t.ClueText) - 1, stdin)) == NULL) {
        printf("Error reading Clue Text\n");
        exit(-1);
    }
    // Remove the newline character from the clue text
    size_t len = strlen(t.ClueText);
    if (len > 0 && t.ClueText[len - 1] == '\n') {
        t.ClueText[len - 1] = '\0';
    }

    printf("Enter Value: ");
    if ((scanf("%d", &t.Value)) != 1) {
        printf("Error reading Value\n");
        exit(-1);
    }
    getchar();  // Clear newline left by previous scanf
    // Writing the structure in the file
    if(write(f, &t, sizeof(Treasure))<0)
    {
        printf("Error writing\n");
        exit(-1);
    }
    printf("Treasure added successfully!\n");
    if(close(f)<0)
    {
        printf("Error closing the file\n");
        exit(-1);
    }  // Close the file
}
void list_treasures(const char *hunt_id)
{

}
void view_treasure(const char *hunt_id, char *treasure_id)
{
    int f;
    char buffer[100]; 
    strcpy(buffer, hunt_id);
    strcat(buffer, "/treasure.dat");

    f = open(buffer, O_RDONLY);  // Opening the file for reading and checking if it exists
    if (f < 0) {
        printf("Error opening file\n");
        exit(-1);
    }

    Treasure t;
    int found = 0;
    ssize_t bytes;
    while ((bytes=read(f, &t, sizeof(Treasure))) == sizeof(Treasure)) {
        if (strcmp(t.TreasureID, treasure_id) == 0) {
            printf("Treasure found!\n");
            printf("TreasureID: %s\n", t.TreasureID);
            printf("UserName: %s\n", t.UserName);
            printf("Latitude: %f\n", t.Latitude);
            printf("Longitude: %f\n", t.Longitude);
            printf("ClueText: %s\n", t.ClueText);
            printf("Value: %d\n", t.Value);
            found = 1;
            break; // Stop searching
        }
    }
    if(bytes==-1)
    {
        printf("Error reading the file\n");
        exit(-1);
    }
    if (!found) {
        printf("Didn't find the treasure with id: %s\n", treasure_id);
    }
    printf("Succesfully found and removed treasure\n");
    if(close(f)<0)
    {
        printf("Error closing the file\n");
        exit(-1);
    }  // Close the file
}

void remove_treasure(const char *hunt_id, char *treasure_id)
{
    int f;
    char buffer[100]; 
    strcpy(buffer, hunt_id);
    strcat(buffer, "/treasure.dat");

    f = open(buffer, O_RDWR);  // Opening the file for reading and writing and checking if it exists
    if (f < 0) {
        printf("Error opening file\n");
        exit(-1);
    }
    off_t current_pos=lseek(f,0,SEEK_SET);
    Treasure t;
    int found = 0;
    ssize_t bytes;  
    while ((bytes=read(f, &t, sizeof(Treasure))) == sizeof(Treasure))
    {
        if(strcmp(t.TreasureID,treasure_id)==0)
        {
            // We found the treasure
            found=1;
        }
        else
        {   
            lseek(f,current_pos,SEEK_SET);
            if(write(f,&t,sizeof(Treasure))<0)
            {
                printf("Error writing\n");
                exit(-1);
            }
            current_pos=lseek(f,0,SEEK_CUR);
        }
    }
    if(bytes==-1)
    {
        printf("Error reading the file\n");
        exit(-1);
    }
    if (!found) {
        printf("Didn't find the treasure with id: %s\n", treasure_id);
    }

    if(close(f)<0)
    {
        printf("Error closing the file\n");
        exit(-1);
    }  // Close the file
}
void remove_hunt(const char *hunt_id)
{
   
}
void log_operation(const char *hunt_id, const char *message)
{

}
void create_symlink(const char *hunt_id)
{

}


int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Usage:\n"
                        "--add <hunt_id>\n"
                        "--list <hunt_id>\n"
                        "--view <hunt_id><treasure_id>\n"
                        "--remove_treasure <hunt_id> <treasure_id>\n"
                        "--remove_hunt <hunt_id>\n");
        exit(-1);
    }
    if (strcmp(argv[1], "--add") == 0) {
        add_treasure(argv[2]);
    } else if (strcmp(argv[1], "--list") == 0) {
        list_treasures(argv[2]);
    } else if (strcmp(argv[1], "--view") == 0) {
        view_treasure(argv[2], argv[3]);
    } else if (strcmp(argv[1], "--remove_treasure") == 0) {
        remove_treasure(argv[2], argv[3]);
    } else if (strcmp(argv[1], "--remove_hunt") == 0) {
        remove_hunt(argv[2]);
    } else {
        fprintf(stderr, "Invalid command\n");
        exit(-1);
    }
    return 0;
}