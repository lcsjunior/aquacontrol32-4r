Upload firmware OTA to the smart aquarium device.

## Prerequisites

- The ESP32 device must be powered on, connected to WiFi, and with OTA active (the currently running firmware must have been compiled with ArduinoOTA support)
- The machine running this command must be on the same local network as the device
- `pio` must be available in the PATH

## Steps

1. Read `src/secrets.h` and extract the value defined in `OTA_PASS` (value only, do not display it)

2. Determine the upload port:
   - If `$ARGUMENTS` was provided by the user, use it as the upload port
   - Otherwise, use `smart-aquarium.local`

3. Run the command below, replacing `<OTA_PASS>` with the extracted value and `<port>` with the determined upload port:

```bash
PLATFORMIO_UPLOAD_PROTOCOL=espota PLATFORMIO_UPLOAD_FLAGS="--auth=<OTA_PASS>" pio run -t upload --upload-port <port>
```

4. Report the result to the user:
   - If the upload completed successfully, indicate that the device is rebooting
   - If there was a connection error (host not found, timeout), check whether the device is on the network and whether the hostname/IP is correct
   - If there was an authentication error, check the value of `OTA_PASS` in `src/secrets.h`
