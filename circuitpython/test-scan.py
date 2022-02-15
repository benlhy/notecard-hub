
from adafruit_ble import BLERadio
from adafruit_ble.advertising import Advertisement
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.envsvc import ENVService

import time
ble = BLERadio()
 

print("scanning")
found = set() # to hash the addresses
devices = [] # to store the entries
for entry in ble.start_scan(ProvideServicesAdvertisement,timeout=None,minimum_rssi=-80,interval=0.1,window=0.01):
    addr = entry.address
    if ENVService in entry.services:
        print("Found an env advertisement")
    
    if addr not in found:
       print(entry.complete_name)
       print(entry.short_name)
       print(entry.address)
       
       if addr == "a4:c1:38:33:e2:d3":
            print("Entry found")
            ble.stop_scan()
            env_connection = ble.connect(entry)
            while env_connection and env_connection.connected:
                print("Connect successful")
                try:
                    env = env_connection[ENVService]
                    print(env.temp_c)
                    print(env.temp)
                    print(env.hum)
                    #svcs = env_connection.discover_remote_services()
                    #print(svcs)
                except OSError:
                    try:
                        print("OSError encountered")
                        env_connection.disconnect()
                    except:
                        print("Unknown error")
                        pass
                    env_connection = None
                time.sleep(0.3)
            
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