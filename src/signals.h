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

void signal_update(bool connection)
{
//LED_BLUE indicates functioning node
//Blinking orange light indicates a connection to network
//less than 10% duty cycle

/* bool isON;

if(millis()%15){ */
//if there is a connection every 5 seconds there will be a .5s period where the blue light is on
//aka 10% duty cycle
if(connection&&((millis()/500)%10==0){
    
     //   digitalWrite(LED_BLUE, millis()/500 % 2);
    digitalWrite(LED_BLUE, 1);    
}
else{
    digitalWrite(LED_BLUE, 0);
}
}