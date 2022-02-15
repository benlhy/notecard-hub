from adafruit_ble import BLERadio
from adafruit_ble.advertising import Advertisement
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService
import time
ble = BLERadio()

print("scanning")
found = set() # to hash the addresses
devices = [] # to store the entries
for entry in ble.start_scan(timeout=45, minimum_rssi=-100):
    addr = entry.address
    if addr not in found and entry.complete_name != "":
        print(entry.complete_name)
            
    found.add(addr)

"""
ble.stop_scan()
print("scan complete")

env_connection = None
for entry in devices:
    if entry.complete_name == "ATC_33E2D3":
        env_connection = ble.connect(entry)
        while env_connection and env_connection.connected:
            try:
                print(env_connection)
            except OSError:
                try:
                    print("OSError encountered")
                    env_connection.disconnect()
                except:
                    print("Unknown error")
                    pass
                env_connection = None
            time.sleep(0.3)
"""

print("scan done")

