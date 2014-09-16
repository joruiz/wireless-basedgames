#ifndef PLATFORM_H
#define PLATFORM_H

//IO pin defines
#define BATTERY_PIN  A0
#define LED_GREEN    29
#define LED_RED      28
#define LED_BLUE     27
#define LED_AMBAR    26
#define DEBUG1       A6
#define DEBUG2       A7

//Serials
#define wifiSerial Serial1
#define gameSerial Serial2

//I2C addresses
#define RFIDREADER_ADDRESS     0x46
#define SOUNDPLAYER_ADDRESS    0x61
#define DISPLAY_ADDRESS        0x51

//INTERRUPT LINES
#define CONNECTED        2
#define RFID_DREADY      7
#define BUTTONS_CHANGE   8

#endif
