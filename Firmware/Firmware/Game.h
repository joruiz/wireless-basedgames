#ifndef GAME_H
#define GAME_H

#include "Arduino.h"

#define NO_GAME  0x00

class StaticGame {

public:
  StaticGame();
  void begin();
  uint8_t loadGame(int timeout);
  boolean start();
  void finish();
  uint8_t available();
  String readMessage();
  void writeMessage(String message);

private:
  String _dataReceived;
  uint8_t _gameId;
  
  void readExisting();
};

extern StaticGame Game;

#endif

