#pragma once
#include "Arduino.h"
#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <avr/boot.h>
#include <EEPROM.h>


#define CH_TELEMETRY 120
#define CH_HEARTBEAT 66
#define CH_SYSMSG 67
#define CH_COMMANDS 68


class WaggleNode {
    public:
        // The Infra
        RF24 radio;
        RF24Network network;
        RF24Mesh mesh;
        
        WaggleNode(uint8_t CE_pin, uint8_t CS_pin);
        bool begin();
        bool begin(uint8_t radio_channel);
        void update();
        uint8_t send_telemetry(void* payload, uint8_t len, uint8_t channel = CH_TELEMETRY, nodeid_t dest);
        nodeid_t nodeID;
    private:
        uint8_t write_(void *payload, uint8_t ch, uint8_t len, nodeid_t dest);
        uint32_t get_sig_byte_();
};
