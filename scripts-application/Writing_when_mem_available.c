#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>


#define DATA_BYTES    8192
#define DATA_WORDS    (DATA_BYTES/4)




// Function to read the i-th line (0-indexed)
char* read_element_at(int target_index) {
    FILE *file = fopen("Global_array.txt", "r"); // "r" mode for reading
    
    if (file == NULL) {
        return "Error opening file!\n";
    }

    static char buffer_temp[256]; // Adjust size if your lines are longer than 255 characters
    int current_index = 0;
    int found = 0;

    // Read line by line
    while (fgets(buffer_temp, sizeof(buffer_temp), file) != NULL) {
        if (current_index == target_index) {
            // Remove the trailing newline character (\n) if it exists
            buffer_temp[strcspn(buffer_temp, "\n")] = '\0';
            fclose(file);
            return buffer_temp; 
        }
        current_index++;
    }

    if (!found) {
        printf("Index %d is out of bounds (file has only %d elements).\n", target_index, current_index);
    }

    fclose(file);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void write_element_at(int target_index, const char *new_value) {
    FILE *original = fopen("Global_array.txt", "r");
    FILE *temp = fopen("temp_array.txt", "w"); // Temporary file
    
    if (original == NULL || temp == NULL) {
        printf("Error opening files!\n");
        return;
    }

    char buffer[256];
    int current_index = 0;
    int modified = 0;

    // Read through the original file line by line
    while (fgets(buffer, sizeof(buffer), original) != NULL) {
        if (current_index == target_index) {
            // Found the index! Write the new value instead of the old line
            fprintf(temp, "%s\n", new_value);
            modified = 1;
        } else {
            // Otherwise, copy the original line exactly as it is
            fprintf(temp, "%s", buffer);
        }
        current_index++;
    }

    
    fclose(original);
    fclose(temp);

    // Swap the temporary file into the original file's place
    remove("Global_array.txt");
    rename("temp_array.txt", "Global_array.txt");
    
    printf("Successfully updated index %d!\n", target_index);
}



int main(int argc, char *argv[]) {

int four_last_dec_digits;
uint64_t address ;
uint32_t buffer[DATA_WORDS];
bool controller=true;
while (controller==true){
printf("Give me the adress you want to write from the FPGA's BRAM (in hex format, e.g. 0xC0000000): ");
scanf("%lx", &address); 
 four_last_dec_digits= address & 0xFFFF;
if (address < 0xC0000000 || address > 0xC0003FFF) 
    printf("Error: Address out of range. Please provide an address between 0xC0000000 and 0xC0001FFF.\n");

else if (strcmp(read_element_at(four_last_dec_digits), "False") == 0)
    printf("Error: Address already used. Please provide a different address.\n");

else
controller=false;
}

write_element_at(four_last_dec_digits, "False");
system("sudo chmod 666 /home/user0000/Same_adress_writting/Global_array.txt");
ssize_t rc;

// Fill the buffer with data
for (int i = 0; i < DATA_WORDS; i++) { buffer[i] = (DATA_WORDS - i); }

printf("Buffer Contents before H2C write: \n");
printf("[0]=%04d, [4]=%04d, [%d]=%04d\n",
        (uint32_t)buffer[0], (uint32_t)buffer[4],
        (DATA_WORDS - 3), (uint32_t)buffer[(DATA_WORDS - 3)]);

// Open M_AXI H2C Host-to-Card Device as Write-Only
int xdma_h2cfd = open("/dev/xdma0_h2c_0", O_WRONLY);

// Write the full buffer to the FPGA design's BRAM
rc = pwrite(xdma_h2cfd, buffer, DATA_BYTES, address);

// Clear the buffer to make sure data was read from FPGA
printf("\nClearing buffer.\n");
for (int i = 0; i < DATA_WORDS ; i++) { buffer[i] = 0; }

// Open M_AXI C2H Card-to-Host Device as Read-Only
int xdma_c2hfd = open("/dev/xdma0_c2h_0", O_RDONLY);

// Read the full buffer from the FPGA design's BRAM
rc = pread(xdma_c2hfd, buffer, DATA_BYTES, address);

printf("\nBuffer Contents after C2H read: \n");
printf("[0]=%04d, [4]=%04d, [%d]=%04d\n",
        (uint32_t)buffer[0], (uint32_t)buffer[4],
        (DATA_WORDS - 3), (uint32_t)buffer[(DATA_WORDS-3)]);

printf("\nrc = %ld = bytes read from FPGA's BRAM\n", rc);

close(xdma_h2cfd);
close(xdma_c2hfd);
}