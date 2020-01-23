#define UNO

#include <avr/wdt.h>
#include "Ticker.h"
#include "system.h"
#include "sensing.h"
#include "comm.h"
#include "pinout.h"
#include "signals.h"
#include "dfu.h"

WaggleNode node(RADIO_CE, RADIO_CSN);

void reportTelemetry() {
    Serial.println(F("Updating telemetry..."));
    collectData(node);
}

Ticker systemStatsTimer(updateSystemMonitor, 10000);
Ticker telemetryTimer(reportTelemetry, 5000);

void setup() {
    wdt_enable(WDTO_8S);
    signal_init();
    signal_startup();
    wdt_reset();
    // Setting up everything
    Serial.begin(115200);
    Serial.print(F("RESET>\t"));
    Serial.println(MCUSR, HEX);
    MCUSR &= 0xf0;  // Clear the flag
    Serial.println(F("!>BOOT"));
    // Get DFU started
    if (is_dfu_available()) {
        Serial.println(F("?>D=DFU"));
        uint32_t flag = millis();
        while (millis() < flag + 1000) {
            if (Serial.available() && Serial.read() == 'D') {
                while (1) {
                    dfu_loop();
                    wdt_reset();  // Hush doggy
                }
            }
        }
    }

    // Actually start a bunch of timeouts
    wdt_reset();  // Hush doggy
    Serial.setTimeout(5000);
    analogReference(INTERNAL);
    // Configure Net
    Serial.println(F(">>Starting node"));
    auto result = node.begin();
    signal_connection_begin(result);
    Serial.println(F(">>Starting sensing"));
    wdt_reset();  // Hush doggy
    initSensing();
    transmitSensorList(node);
    initSystemMonitor();
    initSystemControlExperiment();
    Serial.println(F(">>Starting event loop"));
    wdt_reset();  // Hush doggy
    systemStatsTimer.start();
    telemetryTimer.start();
    wdt_reset();  // Hush doggy
}

void loop() {
    wdt_reset();  // Hush doggy
    node.update();
    systemStatsTimer.update();
    telemetryTimer.update();
}
