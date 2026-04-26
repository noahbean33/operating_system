#include <stdio.h>
#include <stdlib.h>


#include <stdio.h>

void compressFile() {
    int count = 1;
    int current, previous;

    // Read the first character
    previous = getchar();
    if (previous == EOF) {
        return;  // Exit if the file is empty
    }

    // Process the rest of the input
    while ((current = getchar()) != EOF) {
        if (current == previous) {
            count++;
        } else {
            // Write the count (as a character) and the previous character
            putchar(count);       // Writing count directly as a byte
            putchar(previous);     // Writing the character
            previous = current;
            count = 1;  // Reset count for the new character
        }
    }

    // Write the last sequence
    putchar(count);
    putchar(previous);
}

void decompressFile() {
    int count;
    int character;

    // Process the input file
    while ((count = getchar()) != EOF && (character = getchar()) != EOF) {
        for (int i = 0; i < count; i++) {
            putchar(character);  // Write the character 'count' times
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <c|d>\n", argv[0]);
        return 1;
    }

    if (argv[1][0] == 'c') {
        // Compress the input
        compressFile();
    } else if (argv[1][0] == 'd') {
        // Decompress the input
        decompressFile();
    } else {
        printf("Invalid option. Use 'c' for compression and 'd' for decompression.\n");
        return 1;
    }

    return 0;
}

