#pragma once

#define CHRG_CHARGING 7
#define CHRG_STANDBY 8

#define BATT_SENS A6
#define SOLAR_SENS A7

#define LED_BLUE 6
#define LED_ORANGE 4

#define RADIO_CE 9
#define RADIO_CSN 10

#define RADIO_CHANNEL 123

// EEPROM parameter locations
#define DFU_AVAIL_LOC 390       // DFU available, 255=yes. else=no.
#define NODE_ID_LOC 391         // Node ID (big endian).
