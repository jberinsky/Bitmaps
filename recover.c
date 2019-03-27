// Jason Berinsky
// CS 50 Problem Set 3
// Recovers bmp data from a camera's memory card

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cs50.h>

int main(int argc, char *argv[])
{
    // Ensures the program is called with a valid file
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover image\n");
        return 1;
    }
    FILE *rec_file = fopen(argv[1], "r");
    if (rec_file == NULL)
    {
        fprintf(stderr, "Cannot open image\n");
        return 2;
    }

    // Gets the size of the file
    fseek(rec_file, 0, SEEK_END);
    long int file_size = ftell(rec_file);
    fseek(rec_file, 0, SEEK_SET);

    unsigned char buffer[512];
    int file_end = 1;
    long long int index = 0;
    int jpg = 0;
    int counter = 0;
    char name[8];
    sprintf(name, "%03i.jpg", counter);
    FILE *img = fopen(name, "w");

    // Iterates through the data file, storing 512 bytes in a buffer, then checking the buffer for jpg header
    do
    {
        if (jpg)
        {
            fwrite(buffer, 512, 1, img);
        }
        file_end = fread(buffer, 512, 1, rec_file);
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            if (!jpg)
            {
                jpg = 1;
            }
            else
            {
                counter++;
                sprintf(name, "%03i.jpg", counter);
                fclose(img);
                img = fopen(name, "w");
            }
        }
    }
    while (file_end);
    fclose(img);
}