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

void log_operation(const char *hunt_id, const char *message)
{
    int f;
    char buffer[100]; 
    strcpy(buffer, hunt_id);
    strcat(buffer, "/logged_hunt.txt");
    // Opening the file for reading and checking if it exists
    f = open(buffer, O_APPEND | O_WRONLY | O_CREAT, 0644);
    if (f == -1) 
    {
        printf("Error opening file\n");
        exit(-1);
    }
    char messagebuf[50];
    strcpy(messagebuf, message);
    if(write(f,messagebuf,strlen(messagebuf))<0)
    {
        printf("Error writing\n");
        exit(-1);
    }
    if(close(f)<0)
    {
        printf("Error closing the file\n");
        exit(-1);
    }  // Close the file
    printf("Operation logged successfully!\n");
}

void create_symlink(const char *hunt_id)
{
    char log_path[50];
    char link[50];
    snprintf(log_path, sizeof(log_path), "%s/logged_hunt.txt", hunt_id);
    snprintf(link, sizeof(link), "logged_hunt-%s", hunt_id);
    // Remove if exists
    unlink(link);
    if (symlink(log_path, link) == -1) 
    {
        printf("Error creating symlink\n");
        exit(-1);
    }
    else
    {
        printf("Symlink created: %s -> %s\n", link, log_path);
    }
}

void add_treasure(const char *hunt_id)
{
    struct stat st = {0};
    if (stat(hunt_id, &st) == -1)
    {
        mkdir(hunt_id, 0700);
    }
    int f;
    char buffer[100];  
    strcpy(buffer, hunt_id);
    strcat(buffer, "/treasure.dat");
    // Check if the file exists and open it for appending and writing
    f = open(buffer, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (f < 0)
    {
        printf("Error opening file\n");
        exit(-1);
    }
    // Reading the structure from the user
    Treasure t;
    printf("Enter Treasure ID: ");
    if ((scanf("%s", t.TreasureID)) != 1)
    {
        printf("Error reading Treasure ID\n");
        exit(-1);
    }

    printf("Enter User Name: ");
    if ((scanf("%s", t.UserName)) != 1)
    {
        printf("Error reading User Name\n");
        exit(-1);
    }

    printf("Enter Latitude: ");
    if ((scanf("%f", &t.Latitude)) != 1)
    {
        printf("Error reading Latitude\n");
        exit(-1);
    }
    printf("Enter Longitude: ");
    if ((scanf("%f", &t.Longitude)) != 1)
    {
        printf("Error reading Longitude\n");
        exit(-1);
    }
    printf("Enter Clue Text: ");
    getchar();  // Clear newline left by previous scanf
    if ((fgets(t.ClueText, sizeof(t.ClueText) - 1, stdin)) == NULL)
    {
        printf("Error reading Clue Text\n");
        exit(-1);
    }
    // Remove the newline character from the clue text
    size_t len = strlen(t.ClueText);
    if (len > 0 && t.ClueText[len - 1] == '\n')
    {
        t.ClueText[len - 1] = '\0';
    }
    printf("Enter Value: ");
    if ((scanf("%d", &t.Value)) != 1)
    {
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
    // Log the operation
    char msg[256];
    sprintf(msg, "Added treasure ID %s by user %s\n", t.TreasureID, t.UserName);
    log_operation(hunt_id, msg);
    // Create a symlink to the log file
    create_symlink(hunt_id);
    if(close(f)<0)
    {
        printf("Error closing the file\n");
        exit(-1);
    }  // Close the file
}
void list_treasures(const char *hunt_id)
{
    int f;
    char buffer[100]; 
    strcpy(buffer, hunt_id);
    strcat(buffer, "/treasure.dat");
    struct stat info;
    // Check if the file exists
    if (stat(buffer, &info) != 0 || !S_ISREG(info.st_mode)) 
    {
        printf("File does not exist\n");
        exit(-1);
    }
    f = open(buffer, O_RDONLY);  // Opening the file for reading and checking if it exists
    if (f < 0) 
    {
        printf("Error opening file\n");
        exit(-1);
    }

    Treasure t;
    ssize_t bytes;
    //Reading the structure from the file until we find the treasure
    // or reach the end of the file
    while ((bytes=read(f, &t, sizeof(Treasure))) == sizeof(Treasure)) 
    {
        // Print the treasure details
        printf("TreasureID: %s\n", t.TreasureID);
        printf("UserName: %s\n", t.UserName);
        printf("Latitude: %f\n", t.Latitude);
        printf("Longitude: %f\n", t.Longitude);
        printf("ClueText: %s\n", t.ClueText);
        printf("Value: %d\n", t.Value);
    }
    //If bytes is -1, it means there was an error reading the file
    if(bytes==-1)
    {
        printf("Error reading the file\n");
        exit(-1);
    }
    // Log the operation
    char msg[256];
    sprintf(msg, "List treasures in Hunt: %s\n", hunt_id);
    log_operation(hunt_id, msg);
    if(close(f)<0)
    {
        printf("Error closing the file\n");
        exit(-1);
    }  // Close the file
}


void view_treasure(const char *hunt_id, char *treasure_id)
{
    int f;
    char buffer[100]; 
    strcpy(buffer, hunt_id);
    strcat(buffer, "/treasure.dat");
    struct stat info;
    // Check if the file exists
    if (stat(buffer, &info) != 0 || !S_ISREG(info.st_mode)) 
    {
        printf("File does not exist\n");
        exit(-1);
    }
    f = open(buffer, O_RDONLY);  // Opening the file for reading and checking if it exists
    if (f < 0) 
    {
        printf("Error opening file\n");
        exit(-1);
    }
    char username[50];
    Treasure t;
    int found = 0;
    ssize_t bytes;
    while ((bytes=read(f, &t, sizeof(Treasure))) == sizeof(Treasure)) 
    {
        if (strcmp(t.TreasureID, treasure_id) == 0)
        {
            printf("Treasure found!\n");
            printf("TreasureID: %s\n", t.TreasureID);
            printf("UserName: %s\n", t.UserName);
            printf("Latitude: %f\n", t.Latitude);
            printf("Longitude: %f\n", t.Longitude);
            printf("ClueText: %s\n", t.ClueText);
            printf("Value: %d\n", t.Value);
            found = 1;
            strcpy(username, t.UserName);
            break; // Stop searching
        }
    }
    if(bytes==-1)
    {
        printf("Error reading the file\n");
        exit(-1);
    }
    if (!found) 
    {
        printf("Didn't find the treasure with id: %s\n", treasure_id);
        exit(-1);
    }
    // Log the operation
    char msg[256];
    sprintf(msg, "View treasure ID %s by user %s\n", treasure_id, username);
    log_operation(hunt_id, msg);
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
    char username[50];
    // Check if the file exists
    struct stat info;
    if (stat(buffer, &info) != 0 || !S_ISREG(info.st_mode)) 
    {
        printf("File does not exist\n");
        exit(-1);
    }
    // Opening the file for reading and checking if it exists
    f = open(buffer, O_RDWR);
    if (f == -1) 
    {
        printf("Error opening file\n");
        exit(-1);
    }
    // Temporary file to store updated treasures
    char buffer2[100]; 
    strcpy(buffer2, hunt_id);
    strcat(buffer2, "/treasure_temp.dat");
    int temp_fd = open(buffer2, O_APPEND | O_WRONLY | O_CREAT, 0644); // Open for appending
    // Check if the temporary file was opened successfully
    if (temp_fd == -1) 
    {
        printf("Error opening temporary file\n");
        exit(-1);
    }
    Treasure t;
    int treasure_found = 0;
    while (read(f, &t, sizeof(Treasure)) == sizeof(Treasure))
    {
        if (strcmp(t.TreasureID,treasure_id)==0) 
        {
            treasure_found = 1;  // Skipping the treasure to remove it
            strcpy(username, t.UserName);
            continue;
        }
        write(temp_fd, &t, sizeof(Treasure));
    }
    if (!treasure_found) 
    {
        printf("Treasure with ID %s not found.\n", treasure_id);
    } 
    else 
    {
        printf("Treasure with ID %s removed successfully.\n", treasure_id);
        // Replace the old file with the new file
        unlink(buffer);
        rename(buffer2, buffer);
    }
    // Log the operation
    char msg[256];
    sprintf(msg, "Deleted treasure ID %s by user %s\n", treasure_id, username);
    log_operation(hunt_id, msg);
    if(close(f)<0)
    {
        printf("Error closing the file\n");
        exit(-1);
    }  // Close the file
    if(close(temp_fd)<0)
    {
        printf("Error closing the temporary file\n");
        exit(-1);
    }  // Close the temporary file
    // Remove the temporary file
}

void remove_hunt(const char *hunt_id)
{
    struct stat info;
    // Check if the directory exists
    if (stat(hunt_id, &info) != 0 || !S_ISDIR(info.st_mode)) 
    {
        printf("Directory does not exist\n");
        exit(-1);
    }
    char file_path[50];
    // Storing the path of the files
    char files[3][50]={"treasure.dat","logged_hunt.txt"};
    for(int i=0; i<2; i++)
    {
        sprintf(file_path,"%s/%s",hunt_id,files[i]);
        // Removing the files
        if (unlink(file_path) == -1) 
        {
            printf("Error unlink\n");
            exit(-1);
        }
    }
    // Removing the directory
    if (rmdir(hunt_id) == -1) 
    {
        printf("Error rmdir");
        exit(-1);
    }
    printf("Hunt '%s' removed.\n", hunt_id);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage:\n"
                        "--add <hunt_id>\n"
                        "--list <hunt_id>\n"
                        "--view <hunt_id><treasure_id>\n"
                        "--remove_treasure <hunt_id> <treasure_id>\n"
                        "--remove_hunt <hunt_id>\n");
        exit(-1);
    }
    if (strcmp(argv[1], "--add") == 0) 
    {
        add_treasure(argv[2]);
    } 
    else if (strcmp(argv[1], "--list") == 0) 
    {
        list_treasures(argv[2]);
    } 
    else if (strcmp(argv[1], "--view") == 0) 
    {
        view_treasure(argv[2], argv[3]);
    } 
    else if (strcmp(argv[1], "--remove_treasure") == 0) 
    {
        remove_treasure(argv[2], argv[3]);
    } 
    else if (strcmp(argv[1], "--remove_hunt") == 0) 
    {
        remove_hunt(argv[2]);
    } 
    else 
    {
        fprintf(stderr, "Invalid command\n");
        exit(-1);
    }
    return 0;
}