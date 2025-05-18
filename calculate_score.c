#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct
{
    char TreasureID[50];
    char UserName[50];
    float Latitude;
    float Longitude;
    char ClueText[100];
    int Value;
} Treasure;

typedef struct
{
    char name[50];
    int score;
} Score;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Not enough arguments\n");
        return 1;
    }
    //
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/treasure.dat", argv[1]); // Construct the file path
    // Check if the file exists
    int fd = open(filepath, O_RDONLY);
    if (fd < 0)
    {
        printf("Error opening treasure.dat\n");
        return 1;
    }

    Treasure t;
    Score *scores = NULL;
    int score_count = 0;
    ssize_t bytes_read;
    int size = 1;
    while ((bytes_read = read(fd, &t, sizeof(Treasure))) == sizeof(Treasure))
    {
        // Allocate memory for scores array
        if ((scores = realloc(scores, size * sizeof(Score))) == NULL)
        {
            printf("Error at realloc\n");
            return 1;
        }
        int found = 0;
        for (int i = 0; i < score_count; i++)
        {
            if (strcmp(scores[i].name, t.UserName) == 0)
            {
                // If the user is found, update the score
                scores[i].score += t.Value;
                found = 1;
                break;
            }
        }
        if (!found)
        {
            // If the user is not found, add a new entry
            size++;
            strcpy(scores[score_count].name, t.UserName);
            scores[score_count].score = t.Value;
            score_count++;
        }
    }
    // If bytes_read is -1, it means there was an error reading the file
    if (bytes_read < 0)
    {
        printf("Error reading file");
        close(fd);
        return 1;
    }
    // Print the scores
    for (int i = 0; i < score_count; i++)
    {
        printf("Hunt: %s | User: %s -> Score: %d\n", argv[1], scores[i].name, scores[i].score);
    }
    free(scores); // Free the allocated memory
    close(fd);
    fflush(stdout); // Flush the output buffer
    return 0;
}