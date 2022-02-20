# Introduction

The Notecard Hub is a proof of concept of designing a general-purpose hub for sending data points from BLE and Lora-enabled devices to the cloud using Notecard.

This Hub will form the basis of my future Notecard projects as it has low-power connectivity options for collecting data, and utilizes Blues Wireless's cloud platform for zero-configuration provisioning and data collection. The data is then piped to AWS services to demonstrate the cloud-to-cloud capability of notehub.io.

# Exploratory Aspects

This project explores a few features:

1. Implmenting the Notecard with BLE and Lora
2. Building a simple backend and dashboard with AWS services
3. Testing the Notecard's power resilience and energy harvesting in the field

# Technologies & Parts

1. For prototyping, we will use the Feather nRF52840 Express for ease of development with an attached Lipo battery.
2. A Lora module is used to further expand the number of communication protocols and range of the unit.
3. A global Notecard (NOTE-NBGL-500) is used with a Notecarrier-AA.
4. For testing BLE, two Mi Thermometers flashed with the ATC advertising code are used
5. For testing LoRa, a LoRa module connected to the Feather is used and will use the same data formwat as the ATC advertising code.

# Journal

1. Update circuitpython on the nRF52840 Feather
2. Add the [notecard library](https://github.com/blues/note-python) to the `lib` folder of the Feather
3. Install [Mu editor](https://codewith.mu/) - issues: doesn't launch
4. The I2C code for the transaction doesn't work
5. Connect a LiPo battery on the Feather and connect the `Batt` pin to the `V+` pin on the NoteCarrier
   1. This prevents brown-outs when it is connecting to the cell tower - which results in Errno: 19 for I2C and empty JSON responses for UART
6. Unfortunately the CircuitPython BLE code is not mature enough to be used effectively with generic devices. Switching to using PlatformIO for programming
7. Consider using vectors for list management
8. The RadioHead library does not work for the nRF52840, as well as numerous recorded difficulties by others in attempting to use it with the Arduino Nano 33 BLE Sense.

# Questions

1. What is the mitigation plan if Blues Wireless does not exist before the 10 years is up?
2. If I didn't want to use Blue's wireless platform - how do I migrate off it completely?
3. What is the data policy for Blue's Wireless?
4. What is the SLA for Cloud Platform?
5. How does the WiFi access point location work?
6. Are there any commercial success stories for the NoteCard?

# References

1. CircuitPython - https://learn.adafruit.com/introducing-the-adafruit-nrf52840-feather/circuitpython
2. LTE antenna - https://www.digikey.sg/en/products/detail/pulselarsen-antennas/W3907B0100/7667474
3. note-arduino - https://github.com/blues/note-arduino/tree/master/examples
4. Bluefruit central - https://github.com/adafruit/Adafruit_nRF52_Arduino/tree/master/libraries/Bluefruit52Lib/examples/Central
5. ATC flashing code - https://github.com/pvvx/ATC_MiThermometer#reading-measurements-in-advertising-mode
6. BLE Generic Access Profile (GAP) reference - https://www.bluetooth.com/specifications/bluetooth-core-specification/
7. Assigned numbers (GAP) - https://www.bluetooth.com/specifications/assigned-numbers/
8. BLE ADV Reference - https://docs.silabs.com/bluetooth/latest/general/adv-and-scanning/bluetooth-adv-data-basics
