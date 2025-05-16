# Beyond Robotix Kahuna Firmware

This is the repository for the Kahuna Mavlink Wifi Module for Ardupilot, PX4 and iNAV.
- The docs can be found here: [Docs](https://beyond-robotix.gitbook.io/docs/kahuna)
- The store page here: [Store](https://www.beyondrobotix.com/products/kahuna)

Specifications:
- Mass: 4g
- Dimensions: 30x18x6mm
- Max Output Power: 20dBm (~112 mW)
- Range: up to 2.5km
- Input Voltage: 5V
- Continuous Current: 20mA
- Peak Current: 250mA
- Connector: JST-GH 6 Pin (compatible with Pixhawk and Cube Telem port)


## Current Binary

The latest release is available in the releases section on the right-hand side. Download the .bin file and upload it to your kahuna via the web UI to update.

### Useful commands:

* ```platformio run``` - process/build all targets
* ```platformio run -e esp12e``` - process/build just the ESP12e target (the NodeMcu v2, Adafruit HUZZAH, etc.)
* ```platformio run -e esp12e -t upload``` - build and upload firmware to embedded board
* ```platformio run -t clean``` - clean project (remove compiled files)

The resulting image(s) can be found in the directory ```.pioenvs``` created during the build process.

### MavLink Submodule

The ```git clone --recursive``` above not only cloned the MavESP8266 repository but it also installed the dependent [MavLink](https://github.com/mavlink/c_library) sub-module. To upated the module (when needed), use the command:

```git submodule update --init```

### Wiring it up

User level (as well as wiring) instructions can be found [here for px4](https://docs.px4.io/en/telemetry/esp8266_wifi_module.html) and [here for ArduPilot](http://ardupilot.org/copter/docs/common-esp8266-telemetry.html)

* Resetting to Defaults: Parameters can be reset to default by jumping J2 in the first 5 seconds after powering the Kahuna.

### MavLink Protocol

The MavESP8266 handles its own set of parameters and commands. Look at the [PARAMETERS](PARAMETERS.md) page for more information.

### HTTP Protocol

There are some preliminary URLs that can be used for checking the WiFi Bridge status as well as updating firmware and changing parameters. [You can find it here.](HTTP.md)
