#include <arduino.h>

// SOFTWARE SERIAL LEFT
#define rxPin 10 // 50 // 20
#define txPin 11 // 51 // 21

#define LED_PIN 13

typedef struct {
    char offset_x;
    char offset_y;
    unsigned char len;
} PACKET_HEADER;

bool checkCRC(byte buf[], int len);
void calculateCRC(byte b);

////////////////////////////////////////////////////
// PACKET INFO

#define PACKET_SIZE (256 + 3 + 2) // data + header + crc

extern unsigned char buffer[PACKET_SIZE];
