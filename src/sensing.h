#include "pnpmanager.h"
#include "sensor.h"
#include "comm.h"

extern SensorManager manager;
void initSensing();
void transmitSensorList(WaggleNode& node);
void collectData(WaggleNode& node);
void writeData(byte* buffer, byte size);
