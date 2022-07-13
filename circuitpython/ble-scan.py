from adafruit_ble import BLERadio
from adafruit_ble.advertising import Advertisement
# from adafruit_ble.advertising.standard import
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService
from adafruit_ble.services.standard.device_info import DeviceInfoService
from adafruit_ble.services.envsvc import ENVService
import time

# returns a list of entries
def scanForDevices():
    ble = BLERadio()
    # CTRL+D to rerun code
    print("scanning")
    found = set() # to hash the addresses
    devices = [] # to store the entries
    for entry in ble.start_scan(timeout=15, minimum_rssi=-100):
        # print(entry.__bytes__())
        addr = entry.address
        name = entry.short_name
        complete_name = entry.complete_name
        # Add it to our set
        if addr not in found and complete_name != None:
            print(addr,name,complete_name)

            found.add(addr)
            devices.append(entry)
    ble.stop_scan()
    print("scan complete")
    return devices




# return list of results
def getInfoFromDevices(list_of_devices):
    ble = BLERadio()
    scanned_info = []
    env_connection = None
    for entry in list_of_devices:
        if "ATC" in entry.complete_name:
            print(f"Connecting to {entry.complete_name}")
            try:
                env_connection = ble.connect(entry)
            except:
                print("Error in connecting!")
            if env_connection and env_connection.connected:
                try:
                    print(f"Device is connected? {env_connection.connected}")
                    entry_info = {"name":entry.complete_name}
                    entry_info["data"] = []
                    if DeviceInfoService in env_connection:
                        print("Device Info Service available")
                        device_service = env_connection[DeviceInfoService]
                        print(device_service.model_number,device_service.manufacturer,device_service.firmware_revision)
                        service_info = {
                            "service":"DvInfSvc",
                            "model_num":device_service.model_number,
                            "manuf":device_service.manufacturer,
                            "rev":device_service.firmware_revision
                        }
                        entry_info["data"].append(service_info)

                    if ENVService in env_connection:
                        print("Env Service available")
                        env_service = env_connection[ENVService]
                        temp_list = list(env_service.temp_c)
                        # reverse and combine the two bytes
                        temp = int.from_bytes(env_service.temp_c,"little")
                        hum = int.from_bytes(env_service.hum,"little")
                        print(f"{temp/10}C,{hum/100}%")
                        service_info = {
                            "service":"EnvSvc",
                            "temp":temp,
                            "hum":hum,
                        }
                        entry_info["data"].append(service_info)
                    print("Data collection complete")
                    env_connection.disconnect()
                    scanned_info.append(entry_info)
                    while env_connection.connected:
                        # wait for connection to close before connecting
                        # print("Still connected")
                        time.sleep(1)



                except OSError:
                    try:
                        print("OSError encountered")
                        env_connection.disconnect()
                    except:
                        print("Unknown error")
                        pass
                    env_connection = None
                time.sleep(1.0)
    return scanned_info




## Attempting to connect


