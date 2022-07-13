# type: ignore

import time
import busio
import board
import notecard
import ble_scan
from notecard import hub


# Set this if using WIFI notecard  - not working atm ~ cannot connect to Wifi network even when it is 2.4Ghz
WIFI = False


# Connect to the notecard over I2C which is for the notecarrier AF
port = busio.I2C(board.SCL, board.SDA)
card = notecard.OpenI2C(port,0,0)

# Set up connection parameters
productUID = "com.gmail.limhanyangb:start"
print("Notecard setup complete")
rsp = hub.set(card, productUID, mode="periodic", sync=True)
print(rsp)
req = {"req":"hub.get"}
rsp = card.Transaction(req)
print(rsp)

# Setup wifi
if WIFI:
    print("Connecting to WiFi")
    req = {"req": "card.wifi", "ssid":"ssdid","password":"password"}
    rsp = card.Transaction(req)
    print(rsp)

def collect_data():
    # add data here
    list_of_devices = ble_scan.scanForDevices()
    print(list_of_devices)
    print("Getting data from devices")
    list_of_collected_data = ble_scan.getInfoFromDevices(list_of_devices)
    #print(list_of_collected_data)

    # format data to be sent as follows :
    # {name : { temp: 21, hum: 41 }, name_2 : {...}}
    data_to_be_sent = {}
    for device_data in list_of_collected_data:
        name = device_data["name"]
        for service in device_data['data']:
            if service['service']=='EnvSvc':
                data = {'temp':service['temp'],'hum':service['hum']}
                data_to_be_sent[name] = data
    print(data_to_be_sent)
    req = {"req": "note.add","body": data_to_be_sent,"sync":True}
    res = card.Transaction(req)

while True:
    print("Loop")
    time.sleep(300)
    while(1):
        collect_data()
        time.sleep(3)
        req= {"req":"hub.sync.status"}
        rsp=card.Transaction(req)
        print(rsp)
        # wait until completed
        if "completed" in rsp and type(rsp['completed'])==int:
            break

    # print("In Loop")
    # print("In Loop")
