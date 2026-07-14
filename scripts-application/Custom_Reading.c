#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define DATA_BYTES    8192
#define DATA_WORDS    (DATA_BYTES/4)

int main(int argc, char *argv[]) {
uint64_t address ;
uint32_t buffer[DATA_WORDS];
printf("Give me the adress you want to read from the FPGA's BRAM (in hex format, e.g. 0xC0000000): ");
scanf("%lx", &address);


ssize_t rc;

int xdma_c2hfd = open("/dev/xdma0_c2h_0", O_RDONLY);

// Read the full buffer from the FPGA design's BRAM
rc = pread(xdma_c2hfd, buffer, DATA_BYTES, address);

printf("\nI read from C2H and the contents are in the specific adress given: \n");
printf("[0]=%04d, [4]=%04d, [%d]=%04d\n",
        (uint32_t)buffer[0], (uint32_t)buffer[4],
        (DATA_WORDS - 3), (uint32_t)buffer[(DATA_WORDS-3)]);

printf("\nrc = %ld = bytes read from FPGA's BRAM\n", rc);

close(xdma_c2hfd);
}