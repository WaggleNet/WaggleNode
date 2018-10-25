#pragma once
#include "Arduino.h"
#include "comm.h"

#define TYPE_WN_NODE_SYSTEM_STATUS 0x001A
#define BATT_SENS A6
#define SOLAR_SENS A7
#define SIG_CHRG 3
#define SIG_STBY 4

struct system_t {
    float solar_volt;
    float battery_volt;
    bool charging;
    bool charge_ready;
};

float computeVoltage(int pin){
  int sensorValue = analogRead(pin);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (1.1*5.3 / 1023.0);
  return voltage;
}

system_t getPowerReadouts() {
    analogReference(INTERNAL);
    if (!(ADMUX >> REFS1)) {
        analogRead(A6);
        analogRead(A6);
        analogRead(A6);
        analogRead(A6);
        analogRead(A6);
    }   // Get my voltage right

    system_t result;
    result.battery_volt = computeVoltage(BATT_SENS);
    result.solar_volt = computeVoltage(SOLAR_SENS);
    result.charging = !digitalRead(SIG_CHRG);
    result.charge_ready = !digitalRead(SIG_STBY);
    return result;
}

void transmit_power_stats(WaggleNode* node) {
    auto readouts = getPowerReadouts();
    uint8_t* buffer = new uint8_t[17+sizeof(readouts)];
    *(uint32_t*)buffer = node->nodeID;
    *(uint32_t*)(buffer+5) = 0; // Sensor ID
    *(uint32_t*)(buffer+9) = TYPE_WN_NODE_SYSTEM_STATUS; // Sensor Type
    buffer[4] = 0;
    buffer[13] = 0;
    buffer[14] = 1; // # of entries
    buffer[15] = 0; // Entry ID
    buffer[16] = sizeof(readouts); // Data size
    memcpy(buffer+17, &readouts, sizeof(readouts));
    node->send_telemetry(buffer, 17+sizeof(readouts));
    delete[] buffer;
}