# AquaControl32-4R

AquaControl32-4R is an intelligent [ESP32 T-Relay](https://lilygo.cc/en-us/products/t-relay) controller for my 80 L tetra aquarium. It is implemented in Arduino and PlatformIO. It monitors water temperature with a DS18B20 sensor, runs a thermostat, and schedules lights and CO2 on/off events.

The system sends data to ThingSpeak. View the dashboard here:

[ThingSpeak dashboard for channel 2421172](https://thingspeak.mathworks.com/channels/2421172)

### Pinout

| Signal                          | GPIO     |
|---------------------------------|----------|
| Heater relay (K1)               | 21       |
| LED bar relay (K2)              | 19       |
| CO2 solenoid relay (K4)         | 5        |
| DS18B20 sensor                  | 22       |
| On-board status LED             | 25       |

### ThingSpeak field mapping

| Field | Data                            |
|-------|---------------------------------|
| 1     | DS18B20 sensor                  |
| 3     | Heater relay state              |
| 5     | LED bar relay state             |
| 6     | CO2 solenoid relay state        |

## Screenshots

This DIY build is housed in a Shako HT200 plastic box (90x140x180mm) and uses a 4x4 socket panel, [ESP32 T-Relay](https://lilygo.cc/en-us/products/t-relay), waterproof DS18B20 sensors, a 12V supply, wires and wago connectors.

![IMG_20240205_144102019_HDR.jpg](assets/images/IMG_20240205_144102019_HDR.jpg)
![IMG_20240205_154955856.jpg](assets/images/IMG_20240205_154955856.jpg)