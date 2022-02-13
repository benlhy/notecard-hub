# type: ignore

import time
import busio
import board
import notecard
# from notecard import hub

port = busio.UART(board.TX, board.RX, baudrate=9600)

nCard = notecard.OpenSerial(port,debug=True)

productUID = "com.gmail.limhanyangb:start"
print("Notecard setup complete")
# rsp = hub.set(nCard, productUID, mode="continuous", sync=True)
req = {"req":"card.version"}


rsp = nCard.Transaction(req)
time.sleep(1)
print(rsp)
req = {}
req = {"req":"hub.set","product":productUID}
rsp = nCard.Transaction(req)
time.sleep(1)

print(rsp)
req={}
req = {"req":"note.add"}
req["body"] = {"temp": 20.0}

rsp = nCard.Transaction(req)
time.sleep(1)
print(rsp)

req={}
req= {"req":"hub.sync"}
rsp=nCard.Transaction(req)
time.sleep(1)
print(rsp)

req= {"req":"hub.sync.status"}
rsp=nCard.Transaction(req)
time.sleep(1)
print(rsp)

while(1):
    time.sleep(3)
    req= {"req":"hub.sync.status"}
    rsp=nCard.Transaction(req)
    print(rsp)
    # print("In Loop")