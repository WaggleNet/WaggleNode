# WaggleNode

This is the firmware for the WaggleNodes, a group of devices that optionally collect data from Smart Sensors, and transmit data over the WaggleNetNet. Files in this repository describe the **business logic**, i.e. the operating behavior of the node, LED blinking patterns, as well as timing of data collection, transmittion and self-diagnosis.

This code do **not** concern the following (even though these still get linked in during compilation):

- Mesh addressing and address allocation over WaggleNetNet: Submodule lib/WaggleNetNet
- Mesh data-link layer operations: Submodule lib/RF24Network
- Low-level radio operations: Submodule lib/RF24
- Sensor Protocol: Submodule external/sensor_protocol, lib/sensor

Please note that files in those submodules are tracked on other repositories, and you should NEVER commit file changes directly under WaggleNode. For more info, learn about Git Submodules.

## Quick start

To run this, you need compatible hardware:

- WaggleNode PCB Versions 0.5 +, under programming jig
- WaggleNet DevBoard P3, Versions 1.0 +

Double check `platformio.ini` for your programming configuration, and use one of the following environments:

- `development` for development and debug messages over the serial port
- `production` for compiling a firmware version used for actual production.

In both cases, please check the [how-to document](https://wagglenet.atlassian.net/wiki/spaces/SPORT/pages/54493246/How-to) on preflight check items and how to operate the tools. Before you can use the device, also make sure that you set up a Node ID at EEPROM location 391-394 (inclusive). If you have your `EESAVE` fuse configured, you should not need to repeat this step across firmware flashes.

## Device Behavior

Detailed behavior is documented inline. Here's a short rundown of what the node will do when powered up.

First, the node turns on and sets up the IO. When the IO is successfully configured, it blinks both LEDs (Orange and Blue) once. Then it attempts to initialize the radio. If the radio is correctly configured, you should see both lights blink once, and the **blue** light blinks twice. If startup is incorrect, **both lights blink twice**, followed by a single blink of the **orange** light. A sensor scan also occurs at this instant, but nothing will be reported.

In normal operation, the node checks for network packets in real time. It collects data from all sensors (***TODO***) every 5 seconds, and reports internal telemetry every 10 seconds. It should also send out a heartbeat packet every 5-10 seconds (***TODO***) to indicate to the router that it is in active communication.

The internal telemetry packet appears as a sensor packet on the rest of the pipeline, except that it has sensor_id set to the node_id.
