#ifndef WIFICLIENT_H
#define WIFICLIENT_H

#include "Arduino.h"
#include "WiFlyGsx.h"
#include "Protocol.h"

class WifiClient {

public:
  void begin(WiFlyGsx * wifi);
  boolean open(String ip, uint16_t port);
  void close();
  boolean registerTable(uint8_t baseId, uint16_t timeout);
  boolean login(String userId, String * name);
  boolean logout();
  boolean checkGame(uint8_t gameId);
  boolean playGame();
  boolean stopGame();
  void writeMessage(String message);
  boolean available();
  String readMessage();

private:
  WiFlyGsx * _wifi;
  String _dataReceived;
  
  void readExisting();
};

extern WifiClient Client;

#endif


