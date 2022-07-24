
#include "MEGA_ROUTER.h"

#include <SoftwareSerial.h>

//extern unsigned char checkCRC(unsigned char buf[], int len);
//extern void calculateCRC(unsigned char b);

#define MODE_1 1
#define MODE_2 2
#define MODE_3 3
#define MODE_4 4

#define NETWORK_MODEL MODE_1

uint16_t crc;

bool checkCRC(byte buf[], int len)
{
  crc = 0xFFFF;
  for (byte i = 0; i < len - 2; i++) {
    calculateCRC(buf[i]);
  }
  if (highByte(crc) == buf[len - 1] && lowByte(crc) == buf[len - 2]) {
    return true;
  } else {
    return false;
  }
}

void calculateCRC(byte b)
{
  crc ^= (uint16_t)b;          // XOR byte into least sig. byte of crc
  for (byte i = 8; i != 0; i--) {    // Loop over each bit
    if ((crc & 0x0001) != 0) {      // If the LSB is set
      crc >>= 1;                    // Shift right and XOR 0xA001
      crc ^= 0xA001;
    }
    else                            // Else LSB is not set
      crc >>= 1;                    // Just shift right
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
}

unsigned char buffer[PACKET_SIZE];

// Set up a new SoftwareSerial object

SoftwareSerial Serial4 = SoftwareSerial(rxPin, txPin);

#define BAUDRATE 57600

#define LED_PIN_2 2
#define LED_PIN_3 3
#define LED_PIN_4 4
#define LED_PIN_5 5
#define LED_PIN_6 6
#define LED_GND 7

void setup()  {
    // Set the baud rate for the SoftwareSerial object
    Serial.begin(115200); // 피시로부터 패킷 입력
    
    Serial1.begin(BAUDRATE); // RIGHT
    Serial2.begin(BAUDRATE); // UP
    Serial3.begin(BAUDRATE); // DOWN    
    Serial4.begin(BAUDRATE);

    pinMode(LED_PIN, OUTPUT);

    pinMode(LED_PIN_2, OUTPUT);
    pinMode(LED_PIN_3, OUTPUT);
    pinMode(LED_PIN_4, OUTPUT);
    pinMode(LED_PIN_5, OUTPUT);
    pinMode(LED_PIN_6, OUTPUT);
    pinMode(LED_GND, OUTPUT);
    digitalWrite(LED_GND, false);
    
}

boolean LED_bToggle = false;
void LED_toggle()
{
//  LED_bToggle = !LED_bToggle;
//  digitalWrite(LED_PIN, LED_bToggle);
}

boolean LED_bToggle2 = false;
boolean LED_bToggle3 = false;
boolean LED_bToggle4 = false;
boolean LED_bToggle5 = false;
boolean LED_bToggle6 = false;
void LED_toggle_2()
{
  LED_bToggle2 = !LED_bToggle2;
  digitalWrite(LED_PIN_2, LED_bToggle2);
}


void LED_toggle_3()
{
  LED_bToggle3 = !LED_bToggle3;
  digitalWrite(LED_PIN_3, LED_bToggle3);
}

void LED_toggle_4()
{
  LED_bToggle4 = !LED_bToggle4;
  digitalWrite(LED_PIN_4, LED_bToggle4);
}

void LED_toggle_5()
{
  LED_bToggle5 = !LED_bToggle5;
  digitalWrite(LED_PIN_5, LED_bToggle5);
}

void LED_toggle_6()
{
  LED_bToggle6 = !LED_bToggle6;
  digitalWrite(LED_PIN_6, LED_bToggle6);
}


int buIndex = 0;

void Serial_write(unsigned char * ptr, int len)
{
  char x = ptr[0];
  char y = ptr[1];

  #if NETWORK_MODEL == MODE_1

    if(0 <= x && 0 <= y) {}
    else {
      char * pStr = "   MODE_1 # INVALID VECTOR";
      Serial.write(pStr, strlen(pStr));
      return;
    }
    
  #endif

  

  if(x == 0 && y == 0) {
    Serial.write(ptr, len);
    LED_toggle_2();
  }
  else if(0 < x) {
    x -= 1;
    ptr[0] = x;

    crc = 0xFFFF;
    for (byte i = 0; i < len - 2; i++) {
      calculateCRC(ptr[i]);
    }
    ptr[len - 1] = highByte(crc);
    ptr[len - 2] = lowByte(crc);
    
    Serial1.write(ptr, len);

    LED_toggle_3();
    
  }
  else if(x < 0) {
    
    x += 1;
    ptr[0] = x;
        crc = 0xFFFF;
    for (byte i = 0; i < len - 2; i++) {
      calculateCRC(ptr[i]);
    }
    ptr[len - 1] = highByte(crc);
    ptr[len - 2] = lowByte(crc);
    
    Serial4.write(ptr, len);
//    Serial.write(ptr, len);
    
    LED_toggle_6();
  }  
  else if(0 < y) {
    y -= 1;
    ptr[1] = y;
    crc = 0xFFFF;
    for (byte i = 0; i < len - 2; i++) {
      calculateCRC(ptr[i]);
    }
    ptr[len - 1] = highByte(crc);
    ptr[len - 2] = lowByte(crc);
    Serial2.write(ptr, len);

    LED_toggle_4();
  }    
  else if(y < 0) {
    y += 1;
    ptr[1] = y;
        crc = 0xFFFF;
    for (byte i = 0; i < len - 2; i++) {
      calculateCRC(ptr[i]);
    }
    ptr[len - 1] = highByte(crc);
    ptr[len - 2] = lowByte(crc);
    Serial3.write(ptr, len);

    LED_toggle_5();
  }    
}

void loop()
{
      memset(buffer, 0x00, PACKET_SIZE);
      int pos = 0;
      int len = 0;
      while(0 < Serial.available())
      {
        unsigned char val = Serial.read();
        buffer[pos] = val; pos += 1;
        LED_toggle();
        if(0 == Serial.available()) delay(2);

        LED_toggle_2();
      }

      if(0 < pos) {
          Serial_write(buffer, pos);
      }

      memset(buffer, 0x00, PACKET_SIZE);
      pos = 0;
      while(0 < Serial1.available())
      {
        unsigned char val = Serial1.read();
        buffer[pos] = val; pos += 1;
        LED_toggle();
        if(0 == Serial1.available()) delay(2);

        LED_toggle_3();
      }
      if(0 < pos) {
        bool bCrc = checkCRC(buffer, pos);
        if(bCrc == 1) {
//          for(int i = 0; i < pos; i++) {
//            Serial.print(buffer[i], HEX); Serial.print(',');
//          }
//          Serial.println();
            Serial_write(buffer, pos);
        }
        else {
          Serial.println("Serial1 - CRC ERROR!");
        }
      }

      memset(buffer, 0x00, PACKET_SIZE);
      pos = 0;
      while(0 < Serial2.available())
      {
        unsigned char val = Serial2.read();
        buffer[pos] = val; pos += 1;
        LED_toggle();
        if(0 == Serial2.available()) delay(2);
      }
      if(0 < pos) {
        bool bCrc = checkCRC(buffer, pos);
        if(bCrc == 1) {
//          for(int i = 0; i < pos; i++) {
//            Serial.print(buffer[i], HEX); Serial.print(',');
//          }
//          Serial.println();

            Serial_write(buffer, pos);
        }
        else {
          Serial.println("Serial2 - CRC ERROR!");
        }
      }

      memset(buffer, 0x00, PACKET_SIZE);
      pos = 0;
      while(0 < Serial3.available())
      {
        unsigned char val = Serial3.read();
        buffer[pos] = val; pos += 1;
        LED_toggle();
        if(0 == Serial3.available()) delay(2);
      }
      if(0 < pos) {
        bool bCrc = checkCRC(buffer, pos);
        if(bCrc == 1) {
//          for(int i = 0; i < pos; i++) {
//            Serial.print(buffer[i], HEX); Serial.print(',');
//          }
//          Serial.println();

            Serial_write(buffer, pos);
        }
        else {
          Serial.println("Serial3 - CRC ERROR!");
        }
      }

      memset(buffer, 0x00, PACKET_SIZE);
      pos = 0;
      while(0 < Serial4.available())
      {
        unsigned char val = Serial4.read();
        buffer[pos] = val; pos += 1;
        LED_toggle();
        if(0 == Serial4.available()) delay(1);
      }
      if(0 < pos) {
        bool bCrc = checkCRC(buffer, pos);
        if(bCrc == 1) {
//          for(int i = 0; i < pos; i++) {
//            Serial.print(buffer[i], HEX); Serial.print(',');
//          }
//          Serial.println();

            Serial_write(buffer, pos);
        }
        else {
          Serial.println("Serial4 - CRC ERROR!");
        }
      }
}
