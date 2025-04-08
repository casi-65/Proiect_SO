#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int TreasureID;
    char UserName[50];
    float Latitude;
    float Longitude;
    char ClueText[100];
    int Value;
}Treasure;

void add()
{
    FILE *f;
    char treasureName[100];
    // Reading the file name from the user
    printf("Enter the file name: ");
    if((scanf("%s",treasureName))!=1)
    {
        printf("Error reading file name\n");
        exit(-1);
    }
    // Open the file in append mode
    // If the file does not exist, it will be created
    if((f=fopen(treasureName,"w+"))==NULL)
    {
        printf("Error opening file\n");
        exit(-1);
    }
    Treasure t;
    // Reading treasure details from the user
    // Write the treasure details to the file
    printf("Enter Treasure ID: ");
    if((scanf("%d", &t.TreasureID))!=1)
    {
        printf("Error reading Treasure ID\n");
        exit(-1);
    }
    printf("Enter User Name: ");
    if((scanf("%s", t.UserName))!=1)
    {
        printf("Error reading User Name\n");
        exit(-1);
    }
    printf("Enter Latitude: ");
    if((scanf("%f", &t.Latitude))!=1)
    {
        printf("Error reading Latitude\n");
        exit(-1);
    }
    printf("Enter Longitude: ");
    if((scanf("%f", &t.Longitude))!=1)
    {
        printf("Error reading Longitude\n");
        exit(-1);
    }
    printf("Enter Clue Text: ");
    getchar(); // Clear the newline character from the buffer
    // Reading the clue text, even with spaces
    if((fgets(t.ClueText, sizeof(t.ClueText)-1 , stdin))==NULL)
    {
        printf("Error reading Clue Text\n");
        exit(-1);
    }
    // Remove the newline character from the clue text
    size_t len = strlen(t.ClueText);
    if (len > 0 && t.ClueText[len - 1] == '\n') {
        t.ClueText[len - 1] = '\0';
    }
    printf("Enter Value: ");
    if((scanf("%d", &t.Value))!=1)
    {
        printf("Error reading Value\n");
        exit(-1);
    }
    // Write the treasure details to the file
    if((fputs("Treasure ID: ", f))==EOF)
    {
        printf("Error writing to file\n");
        exit(-1);
    }
    if((fprintf(f, "%d\n", t.TreasureID))<0)
    {
        printf("Error writing to file\n");
        exit(-1);
    }
    if((fputs("User Name: ", f))==EOF)
    {
        printf("Error writing to file\n");
        exit(-1);
    }
    if((fprintf(f, "%s\n", t.UserName))<0)
    {
        printf("Error writing to file\n");
        exit(-1);
    }
    if((fputs("Latitude: ", f))==EOF)
    {
        printf("Error writing to file\n");
        exit(-1);
    }
    if((fprintf(f, "%f\n", t.Latitude))<0)
    {
        printf("Error writing to file\n");
        exit(-1);
    }
    if((fputs("Longitude: ", f))==EOF)
    {
        printf("Error writing to file\n");
        exit(-1);
    }
    if((fprintf(f, "%f\n", t.Longitude))<0)
    {
        printf("Error writing to file\n");
        exit(-1);
    }
    if((fputs("Clue Text: ", f))==EOF)
    {
        printf("Error writing to file\n");
        exit(-1);
    }
    if((fprintf(f, "%s\n", t.ClueText))<0)
    {
        printf("Error writing to file\n");
        exit(-1);
    }
    if((fputs("Value: ", f))==EOF)
    {
        printf("Error writing to file\n");
        exit(-1);
    }
    if((fprintf(f, "%d\n", t.Value))<0)
    {
        printf("Error writing to file\n");
        exit(-1);
    }
    // Close the file
    if(fclose(f)!=0)
    {
        printf("Error closing file\n");
        exit(-1);
    }
    printf("Treasure details added successfully\n");
}

int main(void)
{
    add();
    return 0;
}