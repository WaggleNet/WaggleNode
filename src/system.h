#pragma once
#include "Arduino.h"
#include "sensor.h"
#include "sensing.h"
#include "comm.h"
#include "pinout.h"

#define TYPE_WN_NODE_SYSTEM_STATUS 0x001A

Sensor systemMonitorSensor(4);

float solar_volt;
float battery_volt;
bool charging;
bool charge_ready;

float computeVoltage(int pin){
  int sensorValue = analogRead(pin);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (1.1*5.3 / 1023.0);
  return voltage;
}

void initSystemMonitor() {
    systemMonitorSensor.type = TYPE_WN_NODE_SYSTEM_STATUS;
    systemMonitorSensor.address = 0;
    systemMonitorSensor.addFloatEntry(battery_volt);
    systemMonitorSensor.addFloatEntry(solar_volt);
    systemMonitorSensor.addBoolEntry(charging);
    systemMonitorSensor.addBoolEntry(charge_ready);
    manager.addSensor(&systemMonitorSensor);
}

void updateSystemMonitor() {
    analogReference(INTERNAL);
    if (!(ADMUX >> REFS1)) {
        analogRead(A6);
        analogRead(A6);
        analogRead(A6);
        analogRead(A6);
        analogRead(A6);
    }   // Get my voltage right

    battery_volt = computeVoltage(BATT_SENS);
    solar_volt = computeVoltage(SOLAR_SENS);
    charging = !digitalRead(CHRG_CHARGING);
    charge_ready = !digitalRead(CHRG_STANDBY);
    systemMonitorSensor.changed(0);
    systemMonitorSensor.changed(1);
    systemMonitorSensor.changed(2);
    systemMonitorSensor.changed(3);
}
