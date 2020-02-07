#include "dfu.h"
#include "comm.h"

#define DFU_STATE_MAIN 0
#define DFU_STATE_SETID 1
#define DFU_STATE_TESTRADIO 2
#define DFU_STATE_NUKE 255
#define DFU_STATE_HUMAN 200
#define DFU_STATE_HELP 201


uint8_t dfu_state = DFU_STATE_MAIN;
uint8_t dfu_human_mode = false;
uint8_t dfu_update_flag = false;

int8_t parse_hex_from_char(char ch) {
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}

bool is_dfu_available() {
    return EEPROM.read(DFU_AVAIL_LOC) == 255;
}

void dfu_loop() {
    switch(dfu_state) {
        case DFU_STATE_MAIN:
            dfu_main();
            break;
        case DFU_STATE_SETID:
            dfu_setid();
            break;
        case DFU_STATE_TESTRADIO:
            dfu_testradio();
            break;
        case DFU_STATE_HUMAN:
            dfu_human();
            break;
        case DFU_STATE_HELP:
            dfu_help();
            break;
        case DFU_STATE_NUKE:
            dfu_nuke();
            break;
        case DFU_STATE_ACTIVATE:
            dfu_activate();
            break;
        case DFU_STATE_RENEW_ADDRESS:
            dfu_renew_address();
            break;
        case DFU_STATE_SEND_PACKET:
            dfu_send_packet();
            break;
    }
}





void dfu_help() {
    Serial.println(F("DFU Mode Instructions:"));
    Serial.println(F("S=setid       Set Node ID in EEPROM"));
    Serial.println(F("T=test        Test radio and print result"));
    Serial.println(F("N=nuke        Nuke the DFU for good"));
    Serial.println(F("H=human       Enter / Exit human mode; detailed response"));
    Serial.println(F("A=activate    Activate the node"));
    Serial.println(F("R=Renew       Renew node address"));
    Serial.println(F("P=send        Send packet"));
    dfu_state = DFU_STATE_MAIN;
}

void dfu_main() {
    Serial.print(F("DFU>"));
    while(!Serial.available()){
        dfu_activate();
    }
    char c = Serial.read();
    // Echo back the char
    Serial.println(c);
    // Switch to the corresponding state
    switch (c) {
        case 'S': case 's':
            dfu_state = DFU_STATE_SETID;
            break;
        case 'T': case 't':
            dfu_state = DFU_STATE_TESTRADIO;
            break;
        case 'N': case 'n':
            dfu_state = DFU_STATE_NUKE;
            break;
        case 'H': case 'h':
            dfu_state = DFU_STATE_HUMAN;
            break;
        case 'A': case 'a':
            dfu_state = DFU_STATE_ACTIVATE;
        case 'R': case 'r':
            dfu_state = DFU_STATE_RENEW_ADDRESS;
        case 'P': case 'p':
            dfu_state = DFU_STATE_SEND_PACKET; 
        default:
            dfu_state = DFU_STATE_HELP;
            break;
    }
}

void dfu_testradio() {
    WaggleNode node(RADIO_CE, RADIO_CSN);
    int result = node.radio.begin();
    if (dfu_human_mode) {
        if (!result) Serial.println(F("!> Radio failed!"));
        else Serial.println(F("!> Radio is OK!"));
        node.radio.printDetails();
    } else {
        if (!result) Serial.println(F("!>FAIL"));
        else Serial.println(F("!>OK"));
    }
    dfu_state = DFU_STATE_MAIN;
}

uint8_t ds_setid_counter = 0;
uint8_t ds_setid_buffer[4];

void dfu_setid() {
    ds_setid_counter = 0;
    if (dfu_human_mode) {
        Serial.println(F(">>Enter 4-byte, little endian Node ID. Must be zero-padded."));
    }
    Serial.print(F("?>"));
    // Expect 8 characters
    while (ds_setid_counter < 8) {
        if (!Serial.available()) dfu_activate();
        char ch = Serial.read();
        Serial.write(ch);  // Echo back this char
        int8_t data = parse_hex_from_char(ch);
        if (data < 0) {
            // Invalid char, break out
            Serial.println();
            if (dfu_human_mode) {
                Serial.println(F("!> Input is invalid, aborted."));
            } else Serial.println(F("!>ERR"));
            dfu_state = DFU_STATE_MAIN;
            return;
        }
        ds_setid_buffer[ds_setid_counter / 2] <<= 4;  // Left shift the high half-byte
        ds_setid_buffer[ds_setid_counter / 2] += data;  // And add the half byte
        ds_setid_counter ++;
    }
    // Now actually writing the thing into EEPROM
    Serial.println();
    if (dfu_human_mode) {
        Serial.print(F(">> Printout: 0x"));
    } else {
        Serial.print(F(">>"));
    }
    for (uint8_t i = 0; i < 4; i++) {
        Serial.print(' ');
        Serial.print(ds_setid_buffer[i], HEX);
        EEPROM.write(NODE_ID_LOC+i, ds_setid_buffer[i]);
    }
    Serial.println();
    Serial.println(F("!>OK"));
    dfu_state = DFU_STATE_MAIN;
}

void dfu_human() {
    if (dfu_human_mode) {
        dfu_human_mode = false;
        Serial.println(F("!>OK"));
    } else {
        dfu_human_mode = true;
        Serial.println(F("!> Hello human! Any time you're confused, type ?."));
    }
    dfu_state = DFU_STATE_MAIN;
}

void dfu_nuke() {
    // First we durn the DFU fuse down so DFU is no longer possible
    // Unless it's debug mode
    #ifndef DEBUG
    EEPROM.update(DFU_AVAIL_LOC, 0);
    #endif
    // Perform a hard reset
    asm volatile (" jmp 0");
}

void dfu_activate() {
    node.begin()
    //set flag
    dfu_update_flag = true;

    return 0;
}

void dfu_renew_address(){ 
    mesh.renewAddress();
    Serial.println(F("Finish renew address"));
    return 0;
}

void dfu_send_packet(){
    char dest=Serial.read();

    
    // payload :destination address, length, payload
    send_telemetry(dest, strlen(dest), CH_TELEMETRY, 1)    

}