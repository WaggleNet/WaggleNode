#include "manager.h"
#include "sensor.h"
#include "comm.h"

SensorManager manager;

void initSensing() {
    manager.begin();
}

void collectData(WaggleNode* node) {
    // TODO: Loop over every sensor
    manager.collect(0);
    auto sensor = manager.getSensor(0);
    // FIRST PASS: Compute size of message block
    uint8_t block_size = 15; // Headers
    uint8_t num_entry = 0;
    for (int entry = 0; entry < sensor->getSize(); entry++) {
        if (sensor->hasChanged(entry)) {
            block_size += sensor->getLength(entry) + 2;
            num_entry ++;
        }
    }
    if (!num_entry) return;
    // SECOND PASS: Populate the message block
    uint8_t* msg = new uint8_t[block_size];
    // Populate node header
    *(uint32_t*)msg = node->nodeID;
    msg[4] = 0;
    // Populate sensor header
    *(uint32_t*)(msg+5) = sensor->address;
    *(uint32_t*)(msg+9) = sensor->type;
    msg[13] = 0;
    // Populate entry number
    msg[14] = num_entry;
    uint8_t write_head = 15; // Write from here
    for (int entry = 0; entry < sensor->getSize(); entry++) {
        if (sensor->hasChanged(entry)) {
            msg[write_head] = entry;
            msg[write_head+1] = sensor->getLength(entry);
            auto& msg_size = msg[write_head+1];
            memcpy(msg+write_head+2, sensor->getData(entry), msg_size);
            write_head += 2 + msg_size;
        }
    }
    // Transmit the message
    node->send_telemetry(msg, block_size);
    // Clean it up
    delete[] msg;
}