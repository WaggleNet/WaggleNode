#include "pnpmanager.h"
#include "sensor.h"
#include "comm.h"

SensorManager manager;

void initSensing() {
    manager.begin();
}

void transmitSensorList(WaggleNode& node) {
    uint8_t msg_size = manager.getSensorCount()*8 + 7;
    uint8_t msg[msg_size];
    // Prepare the sysmsg header
    // Schema: https://wagglenet.atlassian.net/wiki/spaces/SPORT/pages/13238331/Node-to-Router+Data+Format
    *(uint32_t*)msg = node.nodeID;
    msg[4] = 0;
    uint8_t pos = 5;
    for (int sensor_idx = 0; sensor_idx < manager.getSensorCount(); sensor_idx++) {
        auto sensor = manager.getSensor(sensor_idx);
        *(uint32_t*)(msg+pos) = (uint32_t)sensor->address;
        pos += 4;
        *(uint32_t*)(msg+pos) = (uint32_t)sensor->type;
        pos += 4;
    }
    // Report all the sensors to router
    node.send_telemetry(msg, msg_size, CH_SYSMSG);
}

void collectData(WaggleNode& node) {
    for (int sensor_idx = 0; sensor_idx < manager.getSensorCount(); sensor_idx++) {
        Serial.print(F("-!>\tCOLL\tSensor.Index\t"));
        Serial.println(sensor_idx);
        manager.collect(sensor_idx);
        auto sensor = manager.getSensor(sensor_idx);
        // FIRST PASS: Compute size of message block
        uint8_t block_size = 15; // Headers
        uint8_t num_entry = 0;
        for (int entry = 0; entry < sensor->getSize(); entry++) {
            if (sensor->hasChanged(entry)) {
                block_size += sensor->getLength(entry) + 2;
                num_entry ++;
            }
        }
        Serial.print(F("-!>\tSEND\tMessage.Size\t"));
        Serial.println(block_size);
        Serial.print(F("-!>\tSEND\tMessage.Entries\t"));
        Serial.println(num_entry);
        if (!num_entry) return;
        // SECOND PASS: Populate the message block
        uint8_t* msg = new uint8_t[block_size];
        // Populate node header
        *(uint32_t*)msg = node.nodeID;
        msg[4] = 0;
        // Populate sensor header
        *(uint32_t*)(msg+5) = sensor->address;
        *(uint32_t*)(msg+9) = sensor->type;
        msg[13] = 0;
        // Populate entry number
        msg[14] = num_entry;
        uint8_t write_head = 15; // Write from here
        for (int entry = 0; entry < sensor->getSize(); entry++) {
            if (!sensor->hasChanged(entry)) continue;
            msg[write_head] = entry;
            msg[write_head+1] = sensor->getLength(entry);
            auto& msg_size = msg[write_head+1];
            memcpy(msg+write_head+2, sensor->getData(entry), msg_size);
            write_head += 2 + msg_size;
            Serial.print(F("-!>\tCOLL\tCollect.Entry\t"));
            Serial.println(entry);
            Serial.print(F("-!>\tCOLL\tCollect.Size\t"));
            Serial.println(msg_size);
            // Set the entry as not changed
            sensor->changed(entry, false);
        }
        // Transmit the message
        node.send_telemetry(msg, block_size);
        // Clean it up
        delete[] msg;
    }
}