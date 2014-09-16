#include "Arduino.h"
#include "Protocol.h"

// getCommand
//
// Get the command string of the message
String getCommand(String message) {

  int index = message.indexOf(COMMAND_SEP);
  if (index != -1)
    return message.substring(0, index);
  else
    return message;
}

// getParameters
//
// Get the parameters of the message
String getParameters(String message) {

  int index = message.indexOf(COMMAND_SEP);
  if (index != -1)
    return message.substring(index + 1);
  else
    return "";
}

// numberOfParameters
//
// Get tne number of parameters
int numberOfParameters(String parameters) {

  if (parameters.equals(""))
    return 0;
  else {
    String temp = parameters;
    int count = 0, index;
    do {
      index = temp.indexOf(PARAM_SEP);
      temp = temp.substring(index + 1);
      count++;
    }
    while (index >= 0);
    return count;
  }
}

// getParamenter
//
// Get a parameter defined for its position
String getParameter(String parameters, int position) {

  String temp = parameters, parameter;
  int count = -1, index;
  do {
    index = temp.indexOf(PARAM_SEP);
    parameter = temp.substring(0, index);
    temp = temp.substring(index + 1);
    count++;
  }
  while (count != position);
  return parameter;
}

