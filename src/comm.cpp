#include "comm.h"
#include "sensing.h"
#include "pinout.h"

WaggleNode::WaggleNode(uint8_t CE_pin, uint8_t CS_pin)
    : radio(RF24(CE_pin, CS_pin)), network(RF24Network(radio)), mesh(RF24Mesh(radio, network)) {
}

bool WaggleNode::begin() {
    return begin(RADIO_CHANNEL);
}

bool WaggleNode::begin(uint8_t radio_channel) {
    nodeID = get_sig_byte_();
    mesh.setNodeID(nodeID);
    Serial.print(F("NodeID: "));
    Serial.println(nodeID, HEX);

    Serial.println(F("Connecting to Mesh..."));
    bool result = mesh.begin(radio_channel, RF24_2MBPS);
    return result;
}

void WaggleNode::update() {
    mesh.update();
    while (network.available()) {
        // Received now data
        RF24NetworkHeader header;
        size_t data_size = network.peek(header);
        byte *payload = new byte[data_size];
        network.read(header, payload, data_size);
        // Write data into the sensor
        writeData(payload, data_size);
        delete[] payload;
    }
}

/**
 * Retrieves ATMega328PB Device Unique ID for NodeID
 * Unique ID reside in bytes 14-23 (inclusive), take the last 4
 */
uint32_t WaggleNode::get_sig_byte_() {
    // uint8_t res[4];
    // Take bytes backward to match little endian
    // for (int i = 0; i < 4; i++)
    //     res[23-i] = boot_signature_byte_get(i);
    // return *(uint32_t*)res;
    uint32_t res = 0;
    for (int i = NODE_ID_LOC; i < 395; i++)
        res = (res << 8) | EEPROM[i];
    return res;
}

uint8_t WaggleNode::send_telemetry(void* payload, uint8_t len, uint8_t channel,nodeid_t dest) {
    return write_(payload, channel, len, dest);
}

uint8_t WaggleNode::write_(void *payload, uint8_t ch, uint8_t len, nodeid_t dest) {
    /* Returns:
    - 0: Normal
    - 1: Send failed, test OK
    - 2: Send failed, address is lost (will block)
    - 3: Invalid send call
    */
    if(dest == 0 ){
        auto my_id = mesh.getNodeID(mesh.mesh_address);
        if (my_id < 0 || my_id != nodeID) mesh.renewAddress();
        if (!mesh.write(payload, ch, len)) {
        // If a write fails, check connectivity to the mesh network
        if (!mesh.checkConnection()) {
            // refresh the network address
            Serial.println("Renewing Address");
            mesh.renewAddress();
            return 2;
        } else return 1;
        } else return 0;
    }
    //dest = 1 for dfu send_packet 
    else{
        mesh.renewAddress();
        if (!mesh.write(payload, ch, len)) {
        // If a write fails, check connectivity to the mesh network
        if (!mesh.checkConnection()) {
            // refresh the network address
            Serial.println("Renewing Address");
            mesh.renewAddress();
            return 2;
        } else return 1;
        } else return 0;
        

    }
}
