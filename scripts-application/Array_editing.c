#include <stdio.h>

int main() {
    
    FILE *file = fopen("Global_array.txt", "a");
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }
 
    for (int i = 0; i < 16384; i++) {
        fprintf(file, "True\n");
    }

    fclose(file);
    return 0;
}