#include "Game.h"
#include "Protocol.h"
#include "Hardware.h"

StaticGame::StaticGame() {
}

void StaticGame::begin() {

  gameSerial.begin(9600);
  this->_gameId = NO_GAME;
}

uint8_t StaticGame::loadGame(int timeout) {

  long startTime = millis();

  this->readExisting();
  _dataReceived = "";
  this->writeMessage("GET_GAMEID");
  do {
    delay(5);
    if (this->available()){
      String reply = this->readMessage();
#ifdef DEBUG
      Serial.println("Game reply: " + reply);
#endif
      String command = getCommand(reply);
      if (command.equals("GAMEID")) {
        String parameters = getParameters(reply);
        if (numberOfParameters(parameters) == 1){
          uint8_t temp = getParameter(parameters, 1).toInt();
          if (temp != NO_GAME) {
            this->_gameId = temp;
#ifdef DEBUG
            Serial.print("Game ID: ");
            Serial.println(_gameId, DEC);
#endif
            return this->_gameId;
          }
        }
      }
      break;
    }
  } 
  while ((millis() - startTime) < timeout);
  return NO_GAME;
}

boolean StaticGame::start() {

  if (_gameId != NO_GAME) {
    long startTime = millis();

    writeMessage("PLAY");
    do {
      if (this->available()) {
        String reply = readMessage();
#ifdef DEBUG
      Serial.println("Game reply: " + reply);
#endif
        String command = getCommand(reply);
        if (command.equals("OK"))
          return true;    
        break;
      }
    } 
    while ((millis() - startTime) < 2000);
  }
  return false;
}

void StaticGame::finish() {

  this->_gameId = NO_GAME;
  this->_dataReceived = "";
}

void StaticGame::writeMessage(String message){

  gameSerial.print(message);
  gameSerial.write(END_COMMAND);
}

uint8_t StaticGame::available() {

  this->readExisting();

  int count = -1, index = -1;
  do {
    index = this->_dataReceived.indexOf(END_COMMAND, index + 1);
    count++;
  }
  while (index != -1);
  return count;
}

String StaticGame::readMessage() {

  readExisting();

  int index = this->_dataReceived.indexOf(END_COMMAND);
  if (index != -1) {
    String message = this->_dataReceived.substring(0, index);
    this->_dataReceived = this->_dataReceived.substring(index + 1);
    return message;
  }
  else
    return "";
}

void StaticGame::readExisting() {

  while(gameSerial.available()) {
    char data = gameSerial.read();
    this->_dataReceived += data;
  }
}

StaticGame Game = StaticGame();












