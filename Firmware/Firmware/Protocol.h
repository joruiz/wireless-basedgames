#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "Arduino.h"

#define END_COMMAND  '\r'
#define COMMAND_SEP  ' '
#define PARAM_SEP    ','

String getCommand(String message);
String getParameters(String message);
int numberOfParameters(String parameters);
String getParameter(String parameters, int position);

#endif
