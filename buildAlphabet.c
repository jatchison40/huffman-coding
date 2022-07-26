#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHARACTER_COUNT 128

void load_frequency(char *input_filename, int character_frequency[])
{
    FILE *fp = fopen(input_filename, "rb");
    if (fp == NULL)
    {
        printf("Error: Filename could not be opened\n");
        exit(2);
    }
    char c = fgetc(fp);
    while (c != EOF)
    {
        character_frequency[c]++;
        c = fgetc(fp);
    }
    fclose(fp);
}

int main(int argc, char *argv[])
{

    int character_frequency[CHARACTER_COUNT];

    for (int i = 0; i < CHARACTER_COUNT; i++)
    {
        character_frequency[i] = 0;
    }

    for (int i = 1; i < argc; i++)
    {
        load_frequency(argv[i], character_frequency);
    }
    for (int j = 0; j < 128; j++)
    {
        printf("%d,", character_frequency[j]);
    }

    return 0;
}