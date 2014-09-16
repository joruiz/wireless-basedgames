#include "WifiClient.h"

void WifiClient::begin(WiFlyGsx * wifi) {

  this->_wifi = wifi;
  this->_dataReceived = "";
}

boolean WifiClient::registerTable(uint8_t baseId, uint16_t timeout){

  long initTime = millis();

  this->writeMessage("ADD " + String(baseId));
  do {
    delay(1);
    if (this->available()) {
      String reply = this->readMessage();
      String command = getCommand(reply);
      if (command.equals("OK"))
        return true;
      else if (command.equals("ERROR"))
        return false;
    }
  } 
  while ((millis() - initTime) < timeout);
  return false;
}

boolean WifiClient::login(String tagId, String * name){

  long initTime = millis();

  this->writeMessage("LOGIN " + tagId);
  do {
    delay(1);
    if (this->available()) {
      String reply = this->readMessage();
      String command = getCommand(reply);
      if (command.equals("OK")) {
        do {
          delay(1);
          if (this->available()) {
            *name = this->readMessage();
            return true;
          }
        }      
        while ((millis() - initTime) < 2000);
        return false;
      }
      else if (command.equals("ERROR"))
        return false;
    }    
  }
  while ((millis() - initTime) < 2000);
  return false;
}

boolean WifiClient::logout(){

  long initTime = millis();
  this->writeMessage("LOGOUT");
  do {
    delay(1);
    if (this->available()) {
      String reply = this->readMessage();
      String command = getCommand(reply);
      if (command.equals("OK"))
        return true;
      else if (command.equals("ERROR"))
        return false;
    }    
  }
  while ((millis() - initTime) < 2000);
  return false;
}

boolean WifiClient::checkGame(uint8_t gameId){

  long initTime = millis();
  this->writeMessage("GAME " + String(gameId));
  do {
    delay(1);
    if (this->available()) {
      String reply = this->readMessage();
      String command = getCommand(reply);
      if (command.equals("OK"))
        return true;
      else if (command.equals("ERROR"))
        return false;
    }    
  }
  while ((millis() - initTime) < 2000);
  return false;
}

boolean WifiClient::playGame(){

  long initTime = millis();
  this->writeMessage("PLAY");
  do {
    delay(1);
    if (this->available()) {
      String reply = this->readMessage();
      String command = getCommand(reply);
      if (command.equals("OK"))
        return true;
      else if (command.equals("ERROR"))
        return false;
    }    
  }
  while ((millis() - initTime) < 2000);
  return false;
}

boolean WifiClient::stopGame(){

  long initTime = millis();
  this->writeMessage("STOP");
  do {
    delay(1);
    if (this->available()) {
      String reply = this->readMessage();
      String command = getCommand(reply);
      if (command.equals("OK"))
        return true;
      else if (command.equals("ERROR"))
        return false;
    }    
  }
  while ((millis() - initTime) < 2000);
  return false;
}

void WifiClient::writeMessage(String message) {

  this->_wifi->print(message);
  this->_wifi->write(END_COMMAND);
}

boolean WifiClient::available(){

  this->readExisting();

  int count = -1, index = -1;
  do {
    index = this->_dataReceived.indexOf(END_COMMAND, index + 1);
    count++;
  }
  while (index != -1);
  return count;
}

String WifiClient::readMessage() {

  this->readExisting();

  int index = this->_dataReceived.indexOf(END_COMMAND);
  if (index != -1) {
    String message = this->_dataReceived.substring(0, index);
    this->_dataReceived = this->_dataReceived.substring(index + 1);
    return message;
  }
  else
    return "";
}

void WifiClient::readExisting() {

  while(this->_wifi->available()) {
    char data = this->_wifi->read();
    this->_dataReceived += data;
  }
}

WifiClient Client = WifiClient();












