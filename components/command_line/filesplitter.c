#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define BLOCK_SIZE 1000

void split(char *filename, int num)
{
    FILE *infile = fopen(filename, "r");
    void *buffer = malloc(BLOCK_SIZE);

    fseek(infile, 0, SEEK_END);

    long filesize = ftell(infile);
    long partsize = filesize / num;
    long remainder = filesize % num;
    printf("Filesize: %ld bytes\n", filesize);
    printf("Partsize: %ld bytes\n", partsize);
    printf("Remainder: %ld bytes\n", remainder);

    fseek(infile, 0, SEEK_SET);

    for ( int i=0; i<num; i++)
    {
        int part_finished = 0;
        char partfilename[255];
        // video.mp4-part1
        sprintf(partfilename, "%s-part%d", filename, i);
        printf("%s\n", partfilename);
        FILE* partfile = fopen(partfilename, "w");

        long pos;
        while ( !part_finished )
        {
            fread(buffer, BLOCK_SIZE, 1, infile);
            fwrite(buffer, BLOCK_SIZE, 1, partfile);

            pos = ftell(partfile); 
            if ( pos + BLOCK_SIZE >= partsize )
            {
                fread(buffer, partsize - pos, 1, infile);
                fwrite(buffer, partsize - pos, 1, partfile);
                part_finished = 1; 
            }
        }
        if ( i == num-1 ) // last filepart
        {
            // append remainder bytes
            fread(buffer, remainder, 1, infile);
            fwrite(buffer, remainder, 1, partfile);
        }
        fclose(partfile);
    }
    fclose(infile);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <filename> <num-splits>\n", argv[0]);
    } 

    split(argv[1], atoi(argv[2]));
    
    return 0;
}
