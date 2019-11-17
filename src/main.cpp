#define UNO

#include "Ticker.h"
#include "system.h"
#include "sensing.h"
#include "comm.h"
#include "pinout.h"
#include "signals.h"
#include "dfu.h"

WaggleNode node(RADIO_CE, RADIO_CSN);

void reportSysStats() {
    transmit_power_stats(node);
}

void reportTelemetry() {
    collectData(node);
}

Ticker systemStatsTimer(reportSysStats, 10000);
Ticker telemetryTimer(reportTelemetry, 5000);

void setup() {
    // Setting up everything
    Serial.begin(115200);
    Serial.println(F("!>BOOT"));
    // Get DFU started
    if (is_dfu_available()) {
        Serial.println(F("?>D=DFU"));
        uint32_t flag = millis();
        while (millis() < flag + 1000) {
            if (Serial.available() && Serial.read() == 'D') {
                while (1) dfu_loop();
            }
        }
    }
    Serial.setTimeout(5000);
    analogReference(INTERNAL);
    signal_init();
    signal_startup();
    // Configure Net
    Serial.println(F(">>Starting node"));
    auto result = node.begin();
    signal_connection_begin(result);
    Serial.println(F(">>Starting sensing"));
    initSensing();
    transmitSensorList(node);
    Serial.println(F(">>Starting event loop"));
    systemStatsTimer.start();
    telemetryTimer.start();
}

void loop() {
    node.update();
    systemStatsTimer.update();
    telemetryTimer.update();
}
