#ifndef TACHO_H
#define TACHO_H

#include <Arduino.h>

void tachoInit(int pin, int pulsesPerRev);
float tachoGetRPM(void);

#endif
