#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <avr/wdt.h>

#include "pinout.h"
#include "comm.h"

bool is_dfu_available();
void dfu_loop();

void dfu_main();
void dfu_setid();
void dfu_testradio();
void dfu_human();
void dfu_help();
void dfu_nuke();
