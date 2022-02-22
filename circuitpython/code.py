# type: ignore

import time
import busio
import board
import notecard

from notecard import hub

port = busio.UART(board.TX, board.RX, baudrate=9600)

nCard = notecard.OpenSerial(port,debug=False)

productUID = "com.gmail.limhanyangb:start"
print("Notecard setup complete")
rsp = hub.set(nCard, productUID, mode="continuous", sync=True)
print(rsp)


req = {"req":"note.add"}
req["body"] = {"temp": 24.0}
rsp = nCard.Transaction(req)
print(rsp)



while(1):
    time.sleep(3)
    req= {"req":"hub.sync.status"}
    rsp=nCard.Transaction(req)
    print(rsp)
    if "completed" in rsp:
        break
    # print("In Loop")