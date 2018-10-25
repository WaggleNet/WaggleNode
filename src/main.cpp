#include "Ticker.h"
#include "system.h"
#include "comm.h"
#include "pinout.h"

WaggleNode node(9, 10);


void setup() {
    // Setting up everything
    Serial.begin(115200);
    Serial.setTimeout(5000);
    analogReference(INTERNAL);
    // Configure Net
    node.begin();
}

void loop() {
    node.update();
}
