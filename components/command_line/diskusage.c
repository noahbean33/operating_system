#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <ftw.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PATH_MAX_SIZE 4096
#define FILES_ARR_START_SIZE 10

typedef struct {
    char path[PATH_MAX_SIZE];
    size_t size;
} FileEntry;

FileEntry *files;

size_t current_index = 0;
size_t total_number = 0;

int function(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (current_index >= total_number)
    {
        total_number = total_number*2;
        files = realloc(files, total_number * sizeof(FileEntry));
        if (files == NULL)
        {
            perror("Error reallocating files");
            exit(-1);
        }
    }

    size_t filesize = sb->st_size;

    strcpy(files[current_index].path, path);
    files[current_index].size = filesize;

    current_index++;
    return 0;
}

void init_files()
{
    files = (FileEntry *) calloc(FILES_ARR_START_SIZE, sizeof(FileEntry));
    if (files == NULL)
    {
        perror("Files array could not be allocated");
        exit(-1);
    }

    total_number = FILES_ARR_START_SIZE;
}

int sort(const void *first, const void *second)
{
    FileEntry one = *(FileEntry *) first;
    FileEntry two = *(FileEntry *) second;

    if (one.size == two.size)
        return 0;
    if (one.size > two.size)
        return -1;
    if (one.size < two.size)
        return 1;
}

size_t find_max_filesize()
{
    size_t current_max = 0;
    for (size_t i=0; i<current_index; i++)
    {
        if(files[i].size > current_max)
           current_max = files[i].size; 
    }
    return current_max;
}

void print_files()
{
    size_t max = find_max_filesize();
    for (size_t i=0; i<current_index; i++)
    {
        printf("%ld kB\t%s\n", files[i].size/1024, files[i].path);
        int num_blocks = 10*log10(( (float) files[i].size)/log10((float) max ));
        for (int j=0; j<num_blocks; j++)
            printf("█");
        printf("\n");
    }
}    

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <path>\n", argv[0]);
        exit(-1);
    }

    init_files();
    nftw(argv[1], function, 5, 0);

    qsort(files, current_index, sizeof(FileEntry), sort);

    print_files();
}
