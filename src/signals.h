#pragma once

#include <Arduino.h>
#include "pinout.h"

void signal_init() {
    pinMode(LED_ORANGE, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
}

void signal_startup() {
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_ORANGE, HIGH);
    delay(200);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_ORANGE, LOW);
}

void signal_connection_begin(bool result) {
    if (result) {
        digitalWrite(LED_BLUE, 1);
        digitalWrite(LED_ORANGE, 1);
        delay(400);
        digitalWrite(LED_BLUE, 0);
        digitalWrite(LED_ORANGE, 0);
        delay(400);
        digitalWrite(LED_BLUE, 1);
        delay(400);
        digitalWrite(LED_BLUE, 0);
        delay(400);
        digitalWrite(LED_BLUE, 1);
        delay(400);
        digitalWrite(LED_BLUE, 0);
    } else {
        digitalWrite(LED_BLUE, 1);
        digitalWrite(LED_ORANGE, 1);
        delay(400);
        digitalWrite(LED_BLUE, 0);
        digitalWrite(LED_ORANGE, 0);
        delay(400);
        digitalWrite(LED_BLUE, 1);
        digitalWrite(LED_ORANGE, 1);
        delay(400);
        digitalWrite(LED_BLUE, 0);
        digitalWrite(LED_ORANGE, 0);
        delay(400);
        digitalWrite(LED_ORANGE, 1);
        delay(400);
        digitalWrite(LED_ORANGE, 0); 
    }
}