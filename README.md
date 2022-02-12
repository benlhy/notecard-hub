# Introduction
The Notecard Hub is a proof of concept of designing a general-purpose hub for sending data points from BLE and Lora-enabled devices to the cloud using Notecard.

This Hub will form the basis of Notecard projects as it has low-power connectivity options for collecting data, and utilizes Blues Wireless's cloud platform for zero-configuration provisioning and data collection. The data is then piped to AWS services to demonstrate the cloud-to-cloud capability of the 

# Exploratory Aspects
This project explores a few new features:
1. Implmenting the Notecard with BLE and Lora
2. Building a simple backend and dashboard with AWS services
3. Testing the Notecard's resilience in the field

# Technologies
1. For prototyping, we will use the Feather nRF52840 running CircuitPython for ease of development

# Steps
1. Update circuitpython on the nRF52840 Feather
2. Add the [notecard library](https://github.com/blues/note-python) to the `lib` folder of the Feather
3. Install [Mu editor](https://codewith.mu/) - issues: doesn't launch
4. 
# Questions
1. What is the mitigation plan if Blues Wireless does not exist before the 10 years is up?
2. If I didn't want to use Blue's wireless platform - how do I migrate off it completely?
3. What is the data policy for Blue's Wireless?
4. What is the SLA for Cloud Platform?
5. What LTE antenna should I use?

# References
1. CircuitPython - https://learn.adafruit.com/introducing-the-adafruit-nrf52840-feather/circuitpython