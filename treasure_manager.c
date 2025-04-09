#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    
}
void list_treasures(const char *hunt_id)
{

}
void view_treasure(const char *hunt_id, char *treasure_id)
{

}
void remove_treasure(const char *hunt_id, char *treasure_id)
{

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