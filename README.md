# PDGE4

This project comprises only the hardware programming and schematics. For server-side code, see https://github.com/Ruttmann/FurbMonitoramentoWEB

## This project is a device based on ATMega 2560 that has the capabilities to:
- Receive an IR signal from a remote controller (TV, ar-conditioner, etc.);
- Send it to a external server;
- Monitors the status of an equipment (turned on, off);
- Communicates to the server when the status changes, retrieving via Internet the learned signal to turn on/off the equipment.
- Emit those IR signals, turning on/off the equipments in the room.

The server can have infinite IR signals decodified and stored, and has a simple web interface for:
- Monitor availability of the devices that joined to him;
- Monitor possible operation failures of the devices (during the status monitoring of the equipments);
- Issue a command to one or more devices (TO BE DEVELOPED);
- Assign IR signals to devices, making possible to change devices between rooms, for example.

## Main hardware used:
- Arduino ATMega 2560;
- Ethernet Shield W5100;
- IR receptor;
- IR emitter diodes;
- Common electronic components (see schematic on project files).


## External Dependencies

- [Socket.io-v1.x-Library](https://github.com/washo4evr/Socket.io-v1.x-Library)
- [Arduino-IRremote](https://github.com/z3t0/Arduino-IRremote)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
