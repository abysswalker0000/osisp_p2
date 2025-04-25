#include <stdio.h>
#include <string.h>
#include "utils.h"

int main(int argc, char *argv[]) {
    FILE *output = stdout;

    if (argc >= 3 && strcmp(argv[1], "-o") == 0) {
        output = fopen(argv[2], "w");
        if (output == NULL) {
            perror("fopen");
            return 1;
        }
    } else if (argc != 1) {
        fprintf(stderr, "Usage: %s [-o output_file]\n", argv[0]);
        return 1;
    }


    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0'; // Удаляем '\n'
        }
        reverse(buffer); 
        fprintf(output, "%s\n", buffer);
    }
    if (output != stdout) {
        fclose(output);
    }
    return 0;
}