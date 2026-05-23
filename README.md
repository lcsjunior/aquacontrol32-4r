# AquaControl32-4R

AquaControl32-4R is an intelligent ESP32 LILYGO T-Relay controller for my 80 L tetra aquarium. It is built with Arduino code and managed with PlatformIO. It monitors water temperature with a DS18B20 sensor, runs a thermostat, and schedules lights and CO2 on/off events.

The system sends data to ThingSpeak. View the dashboard here: <https://thingspeak.com/channels/2421172>

### Relay outputs

| Relay | Function |
|------|----------|
| K1   | Heater 1 |
| K2   | Heater 2 |
| K3   | LED bars |
| K4   | CO2 solenoid valve |

## Screenshots

This DIY build is housed in a Shako HT200 plastic box and uses a 4x4 socket panel, LILYGO T-Relay ESP32, waterproof DS18B20 sensors, a 12V supply and wires.

![IMG 20240205 154955856](extras/img/IMG_20240205_154955856.jpg)
![IMG 20240205 144102019 HDR](extras/img/IMG_20240205_144102019_HDR.jpg)
