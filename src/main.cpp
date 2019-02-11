#define UNO

#include "Ticker.h"
#include "system.h"
#include "sensing.h"
#include "comm.h"
#include "pinout.h"
#include "signals.h"

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
    Serial.setTimeout(5000);
    analogReference(INTERNAL);
    signal_init();
    signal_startup();
    // Configure Net
    auto result = node.begin();
    signal_connection_begin(result);
    initSensing();
    systemStatsTimer.start();
    telemetryTimer.start();
}

void loop() {
    node.update();
    systemStatsTimer.update();
    telemetryTimer.update();
}
