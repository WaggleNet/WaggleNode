#pragma once
#include "Arduino.h"
#include "comm.h"
#include "pinout.h"

#define TYPE_WN_NODE_SYSTEM_STATUS 0x001A

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
    result.charging = !digitalRead(CHRG_CHARGING);
    result.charge_ready = !digitalRead(CHRG_STANDBY);
    return result;
}

void transmit_power_stats(WaggleNode& node) {
    auto readouts = getPowerReadouts();
    // Data size
    // Header            15
    // Entry overhead   + 8
    // All entries     + 10
    // --------------------
    //           total = 33
    const size_t data_size = 33;
    uint8_t* buffer = new uint8_t[data_size];
    *(uint32_t*)buffer = node.nodeID;
    *(uint32_t*)(buffer+5) = 0; // Sensor ID
    *(uint32_t*)(buffer+9) = TYPE_WN_NODE_SYSTEM_STATUS; // Sensor Type
    buffer[4] = 0; // reserved
    buffer[13] = 0; // reserved
    buffer[14] = 4; // # of entries
    // Entry 1: Solar volt
    buffer[15] = 0; // Entry ID
    buffer[16] = sizeof(float); // Data size
    memcpy(buffer+17, &(readouts.solar_volt), sizeof(float));
    // Entry 2: Battery volt
    buffer[21] = 1; // Entry ID
    buffer[22] = sizeof(float); // Data size
    memcpy(buffer+23, &(readouts.battery_volt), sizeof(float));
    // Entry 3: Charging
    buffer[27] = 2; // Entry ID
    buffer[28] = sizeof(bool); // Data size
    memcpy(buffer+29, &(readouts.charging), sizeof(bool));
    // Entry 4: Charge ready
    buffer[30] = 3; // Entry ID
    buffer[31] = sizeof(bool); // Data size
    memcpy(buffer+32, &(readouts.charge_ready), sizeof(bool));
    node.send_telemetry(buffer, data_size);
    delete[] buffer;
}