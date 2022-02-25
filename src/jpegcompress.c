#include <stdio.h>
#include <stdlib.h>
int main(int argc, char const *argv[]) {
    FILE *file;
    file = fopen("jpegstructure.jpg", "rt");
    unsigned char *buffer;
    unsigned long fileLen;
    // Get file length
    fseek(file, 0, SEEK_END);
    fileLen = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory
    buffer = (char *)malloc(fileLen);
    if (!buffer) {
        fprintf(stderr, "Memory error!");
        fclose(file);
        return 1;
    }

    fread(buffer, fileLen, sizeof(unsigned char), file);
    fclose(file);

    int i = 0;

    while (i < fileLen) {
           printf("%02X ",((unsigned char)buffer[i]));
           i++;
           if( ! (i % 16) ) printf( "\n" );
    }

    return 0;
}