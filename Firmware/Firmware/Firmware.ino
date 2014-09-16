#include <Wire.h>
#include <SM125.h>
#include <SoundPlayer.h>
#include <GraphicDisplay.h>
#include <WiFlyGsx.h>
#include "Hardware.h"
#include "Protocol.h"
#include "WifiClient.h"
#include "Game.h"

#define TABLE_ID             1

//Battery control constants
#define BATTERY_INTERVAL     10000
#define MIN_BATTERY          10
#define BATTERY_MARGIN       5

//Wifi Control constants
#define WIFI_INTERVAL        2000
#define SERVER_IP            "192.168.0.50"    //server aitor
//#define SERVER_IP            "192.168.0.60"    //server johnny
#define SERVER_PORT          9999

//Game control constants
#define GAME_LOAD_TIMEOUT    3000

//Initialization constants
#define INITIATE_DELAY       250
#define INITIATE_INTERVAL    250
#define INITIATE_TIME        5000

//System status
#define INITIATE             0
#define CONNECT              1
#define LOGIN                2
#define STANDBY              3
#define GAME                 4
#define FATAL_ERROR          5

//debug defines
#define DEBUG
#ifdef DEBUG
//#define DEBUG_BATTERY
//#define DEBUG_WIFI
//#define DEBUG_INIT
//#define DEBUG_LOGIN
#endif

//state variables
byte state = INITIATE;

//variables for initiate state
long initiatePrevTime = -INITIATE_INTERVAL;

//Battery control variables
long batteryPrevTime = -BATTERY_INTERVAL;
int batteryLevel = 0;

//Wifi control variables
long wifiPrevTime = WIFI_INTERVAL;
byte wifiState = NOIP;
boolean wifiConnected = false;

//Peripherals
WiFlyGsx wifi;
SM125 rfidReader;
SoundPlayer soundPlayer;
GraphicDisplay graphicDisplay;

String message, command, parameters, reply;
String userId, userName;

void setup() {

#ifdef DEBUG
  Serial.begin(9600);
  Serial.println("Start Base Firmware V1");
  Serial.println("-> Go to INITIATE state");
  //init debug LEDs
  pinMode(DEBUG1, OUTPUT);     
  digitalWrite(DEBUG1, LOW);
  pinMode(DEBUG2, OUTPUT);     
  digitalWrite(DEBUG2, LOW);
#endif

  //init external lights
  pinMode(LED_GREEN, OUTPUT);     
  digitalWrite(LED_GREEN, HIGH);
  pinMode(LED_RED, OUTPUT);     
  digitalWrite(LED_RED, LOW);
  pinMode(LED_BLUE, OUTPUT);     
  digitalWrite(LED_BLUE, LOW);
  pinMode(LED_AMBAR, OUTPUT);     
  digitalWrite(LED_AMBAR, LOW);

  //init CONNECTED pin
  pinMode(CONNECTED, INPUT_PULLUP);

  //init peripherals
  rfidReader.begin(RFIDREADER_ADDRESS, RFID_DREADY);
  soundPlayer.begin(SOUNDPLAYER_ADDRESS);
  graphicDisplay.begin(DISPLAY_ADDRESS, BUTTONS_CHANGE);
  wifiSerial.begin(9600);
  wifi.begin(&wifiSerial);

  //init ClientObject
  Client.begin(&wifi);

  //init game object
  Game.begin();

//  setupWifi();
}

void loop() {

  //update battery level
  if ((millis() - batteryPrevTime) > BATTERY_INTERVAL){
    batteryPrevTime = millis();
    updateBatteryLevel();
  }

  //execute state algorith
  switch (state)
  {
  case INITIATE:
    //Check Wifi state when the system has been initiated
    if ((wifiState == NOIP) && ((millis() - wifiPrevTime) > WIFI_INTERVAL)) {
      wifiPrevTime = millis();
      updateWifiState();
    }
    if ((millis() - initiatePrevTime) > INITIATE_INTERVAL) {
      initiatePrevTime =  millis();
      if (graphicDisplay.isReady()) {
        graphicDisplay.clear();
        graphicDisplay.printText("Initialization...", 1, 1, GREEN);
        graphicDisplay.printText("> Graphic display ok", 3, 1, GREEN);
#ifdef DEBUG_INIT
        //get version of display firmware
        byte ver = graphicDisplay.getVersion();  
        if (ver != 0xFF) {
          Serial.print("Display initiated correctly: Firmware ");
          switch(ver)
          {
          case 0x10:
            Serial.println("1.0");
            break;
          }
        }
#endif
        delay(INITIATE_DELAY);
        boolean soundPlayerInitiated = false;
        if (initSoundPlayer()) {
          soundPlayerInitiated = true;
          graphicDisplay.printText("> Sound player ok", 4, 1, GREEN);
        }
        else
          graphicDisplay.printText("Hola hola caracola", 4, 1, RED);
        //          graphicDisplay.printText("> Sound player error", 4, 1, RED);
        delay(INITIATE_DELAY);
        boolean rfidReaderInitiated = false;
        if (initRfidReader()) {
          rfidReaderInitiated = true;
          graphicDisplay.printText("> Rfid reader ok", 5, 1, GREEN);
        }
        else
          graphicDisplay.printText("> Rfid reader error", 5, 1, RED);
        boolean wifiInitiated = false;
        if (initWifiModule()) {
          wifiInitiated = true;
          graphicDisplay.printText("> Wifi module ok", 6, 1, GREEN);
        }
        else
          graphicDisplay.printText("> Wifi module error", 6, 1, RED);
        if (soundPlayerInitiated && rfidReaderInitiated && wifiInitiated){
          delay(INITIATE_DELAY * 4);
          state = CONNECT;
#ifdef DEBUG
          digitalWrite(DEBUG1, HIGH);
#endif
          graphicDisplay.clear();
          /*************************************************************************************************/
          /* Sustituir por imagen                                                                          */
          graphicDisplay.printText("THOFU", 6, 8, GREEN);
          graphicDisplay.printText("Conectando con servidor...", 8, 0, GREEN);
          /*************************************************************************************************/
#ifdef DEBUG
          Serial.println("Initiation finalized.");
          Serial.println("-> Go to CONNECT state");
#endif
        }
        else{
          state = FATAL_ERROR;
          digitalWrite(LED_AMBAR, HIGH);
#ifdef DEBUG_INIT
          Serial.println("FATAL ERROR: Peripherical initialization error");
#endif
        }
      }
    }
    else if (millis() > INITIATE_TIME) {
      state = FATAL_ERROR;
      digitalWrite(LED_AMBAR, HIGH);
#ifdef DEBUG_INIT
      Serial.println("-> FATAL ERROR: Graphis display do not response");
#endif
    }
    break;
  case CONNECT:
    //Check Wifi state when the system is connecting tothe server
    if ((wifiState != CONNECTED) && ((millis() - wifiPrevTime) > WIFI_INTERVAL)) {
      wifiPrevTime = millis();
      updateWifiState();
    }
    if (wifiState == IDLE) 
      if (wifi.openTcp(SERVER_IP, SERVER_PORT)) {
#ifdef DEBUG
        Serial.println("Sockect opened");
#endif
        delay(50);
        if (Client.registerTable(TABLE_ID, 2000)) {
          /*************************************************************************************************/
          /* Actualizar a estado LOGIN                                                                          */
          state = STANDBY;
#ifdef DEBUG
          digitalWrite(DEBUG1, LOW);
#endif
          /*************************************************************************************************/
          graphicDisplay.clear();
          /*************************************************************************************************/
          /* Sustituir por imagen                                                                          */
          graphicDisplay.printText("THOFU", 6, 8, GREEN);
          graphicDisplay.printText("Logueate!!!", 8, 6, GREEN);
          /*************************************************************************************************/
#ifdef DEBUG
          Serial.println("Connection and register finalized.");
          Serial.println("-> Go to LOGIN state");
#endif
        }
        else {
          wifi.closeTcp();
#ifdef DEBUG
          Serial.println("No ha llegado el OK");
#endif
        }
      }
    else{
      wifi.closeTcp();
#ifdef DEBUG
      Serial.println("No se puede abrir el socket");
#endif
    }
    break;
  case LOGIN:
    if (rfidReader.anyTag()) {
      userId = rfidReader.readTag();
#ifdef DEBUG_LOGIN
      Serial.println("Card code read: " + userId);
#endif
      if (Client.login(userId, &userName)) {
        /*************************************************************************************************/
        /* Validar usuario contra el servidor                                                            */
        /*************************************************************************************************/
        state = STANDBY;
        graphicDisplay.clear();
        /*************************************************************************************************/
        /* Sustituir por imagen                                                                          */
        graphicDisplay.printText("THOFU", 6, 8, GREEN);
        graphicDisplay.printText("Esperando juego...", 8, 6, GREEN);
        /*************************************************************************************************/
#ifdef DEBUG
        Serial.println("UserId Accepted. UserName: " + userName);
#endif


#ifdef DEBUG
        Serial.println("Login finalized.");
        Serial.println("-> Go to STANDBY state");
#endif
      }
    }
    break;
  case STANDBY:
    if (digitalRead(CONNECTED) == LOW) {
      byte gameId = Game.loadGame(GAME_LOAD_TIMEOUT);
      if (gameId != NO_GAME) {
#ifdef DEBUG
        Serial.println("GameID: " + String(gameId));
#endif
        if (Client.checkGame(gameId)) {
#ifdef DEBUG
          Serial.println("Server response: OK");
#endif
          if (Game.start()) {
#ifdef DEBUG
            Serial.println("Game empieza");
#endif
            if (Client.playGame()) {
              state = GAME;
#ifdef DEBUG
              Serial.println("Juego validado y a jugar");
              Serial.println("-> Go to GAME state");
#endif
            }
          }
#ifdef DEBUG
          else
            Serial.println("Game no responde PLAY");
#endif

        }
      }
      else
#ifdef DEBUG
        Serial.println("Game Error: no response");
#endif
    }
    if (graphicDisplay.buttonsChanged()) {
      byte buttonsState = graphicDisplay.getButtons();
      if ((buttonsState & 0x10) == 0x00) {
#ifdef DEBUG
        Serial.println("Button RED activated");
#endif
        Client.logout();
        //read a tag to release rfid reader
        rfidReader.readTag();
        state = LOGIN;
#ifdef DEBUG
        Serial.println("User logout");
        Serial.println("-> Go to LOGIN state");
#endif
      }
    }
    break;
  case GAME:
    delay(100);
    if (Client.available()) {
      message = Client.readMessage();
#ifdef DEBUG
      Serial.println("Mensaje a Juego: " + message);
#endif
      Game.writeMessage(message);
    }
    if (Game.available()) {
      message = Game.readMessage();
      command = getCommand(message);
      if (command.equals("GET_BATTERY")) {
        reply = "BATTERY " + String(batteryLevel);
        Game.writeMessage(reply);
      }
      else if (command.equals("GET_CONNECTION")) {
        reply = "CONNECTION " + String(wifi.status());
        Game.writeMessage(reply);        
      }
      else if (command.equals("GET_USER")) {
        reply = "USER " + userId + "," + userName;
        Game.writeMessage(reply);        
      }
      else {
#ifdef DEBUG
        Serial.println("Mensaje a Servidor: '" + message + "'");
#endif
        Client.writeMessage(message);
      }
    }    
    if (digitalRead(CONNECTED) == HIGH) {
      Game.finish();
      Client.stopGame();
      state = STANDBY;
#ifdef DEBUG
      Serial.println("Game removed");
      Serial.println("-> Go to STANDBY state");
#endif
    }
    break;
  case FATAL_ERROR:
    break;
  }
  delay(1);
}

//
// initSoundPlayer
//
// check if the sound player is running correctly
boolean initSoundPlayer() {

  //get version of sound player firmware
  byte ver = soundPlayer.getVersion();  
  if (ver != 0xFF) {
#ifdef DEBUG_INIT
    Serial.print("Sound player initiated correctly: Firmware ");
    switch(ver)
    {
    case 0x10:
      Serial.println("1.0");
      break;
    }
#endif
    return true;
  }
  return false;
}

//
// initRfidReader
//
// check if the rfid reader is running correctly
boolean initRfidReader() {

  if (rfidReader.readInput() != 0xFF) {
#ifdef DEBUG_INIT
    Serial.println("Rfid reader initiated correctly");
#endif
    return true;
  }
  return false;
}

//
// initWifiModule
//
// check if the wifi module is running correctly
boolean initWifiModule() {

  if (wifi.status() != 0xFF){
#ifdef DEBUG_INIT
    Serial.println("Wifi module initiated correctly");
#endif
    return true;
  }
  return false;
}

void updateBatteryLevel() {

  int prevBatteryLevel = batteryLevel;
  int sensorValue = analogRead(BATTERY_PIN);
  // Convert the analog reading (witch goes from 825 - 1000) to battery level (0 - 100%)
  batteryLevel = (sensorValue - 825) * (100.0 / 175.0);
  if (batteryLevel != prevBatteryLevel) {
#ifdef DEBUG_BATTERY
    Serial.print("Battery Level: ");
    Serial.println(batteryLevel, DEC);
#endif
    if (batteryLevel < MIN_BATTERY) {
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, HIGH);
    }
    else if (batteryLevel > (MIN_BATTERY + BATTERY_MARGIN)){
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_RED, LOW);
    }
  }
}

void updateWifiState() {
  byte prevWifiState = wifiState;
  wifiState = wifi.status();
  if (wifiState != prevWifiState) {
#ifdef DEBUG_WIFI
    switch (wifiState)
    {
    case 0:
      Serial.println("WifiState: IDLE");
      break;
    case 1:
      Serial.println("WifiState: CONNECTED");
      break;
    case 3:
      Serial.println("WifiState: NOIP");
      break;
    case 4:
      Serial.println("WifiState: CONNECTING");
      break;
    }
#endif
    if ((wifiState == IDLE) || (wifiState == CONNECTED))
      digitalWrite(LED_BLUE, HIGH);
    else
      digitalWrite(LED_BLUE, LOW);
  }
}

void setupWifi() {

  Serial2.print("Deseas configurar los datos de acceso WiFi? (y/n): ");
  long initTime = millis();
  do {
    if (Serial2.available()) {
      char data = Serial2.read();
      if (data == 'y') {
        Serial2.println();
        Serial2.print("  Teclea el SSID de la red WiFi (ENTER para confirmar): ");
        String ssid = "";
        while(true){
          if (Serial2.available()) {
            data = Serial2.read();
            if (data == '\r')
              break;
            ssid += data;
          }
        }
        Serial2.println();
        Serial2.print("  Clave de acceso de la red WiFi (ENTER para confirmar): ");
        String pass = "";
        while(true){
          if (Serial2.available()) {
            data = Serial2.read();
            if (data == '\r')
              break;
            pass += data;
          }
        }
        Serial2.println();
        Serial2.println("Configurando WiFi...");
        if (wifi.setWlan(ssid, pass))
          Serial2.println("-> Datos de conexion Wifi Actualizados");
        else
          Serial2.println("-> Error al actualizar los datos. Reinicia el sistema e intentalo de nuevo...");
      }
    }
  }
  while ((millis() - initTime) < 1000);
}


















