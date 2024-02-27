#include <stdio.h>

void main() {
    char header[] = {   0x45, 0x00, 0x00, 0x40, 
                        0x7c, 0xda, 0x40, 0x00, 
                        0x80, 0x06, 0xfa, 0xd8,
                        0xc0, 0xa8, 0x0f, 0x0b, 
                        0xbc, 0xac, 0xf6, 0xa4};

    int version = (header[0] >> 4) & 0x0F;
    printf("Version: %d\n", version);
    int ihl = (header[0] & 0x0F) * 4;
    printf("IHL: %d bytes\n", ihl);
    int totalLength = header[2] * 256 + header[3];
    printf("Total length: %d bytes\n", totalLength);
    printf("Source IP: %d.%d.%d.%d\n", 
        (unsigned char)header[12], (unsigned char)header[13], (unsigned char)header[14], (unsigned char)header[15]);
    printf("Dest IP: %d.%d.%d.%d\n", 
        (unsigned char)header[16], (unsigned char)header[17], (unsigned char)header[18], (unsigned char)header[19]);
}