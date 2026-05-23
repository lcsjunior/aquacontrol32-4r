# AquaControl32-4R

AquaControl32-4R is an intelligent [ESP32 LILYGO T-Relay](https://lilygo.cc/en-us/products/t-relay) controller for my 80 L tetra aquarium. It is built with Arduino code and managed with PlatformIO. It monitors water temperature with a DS18B20 sensor, runs a thermostat, and schedules lights and CO2 on/off events.

The system sends data to ThingSpeak. View the dashboard here: <https://thingspeak.com/channels/2421172>

### Relay outputs

| Relay | Function |
|------|----------|
| K1   | Heater 1 |
| K2   | Heater 2 |
| K3   | LED bars |
| K4   | CO2 solenoid valve |

## Screenshots

This DIY build is housed in a Shako HT200 plastic box (90x140x180mm) and uses a 4x4 socket panel, [ESP32 LILYGO T-Relay](https://lilygo.cc/en-us/products/t-relay), waterproof DS18B20 sensors, a 12V supply and wires.

<img src="assets/images/IMG_20240205_154955856.jpg" width="300px" alt="Foto 1" />
<img src="assets/images/IMG_20240205_144102019_HDR.jpg" width="300px" alt="Foto 2" />