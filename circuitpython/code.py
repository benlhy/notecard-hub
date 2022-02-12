import busio
import board
import notecard

port = busio.I2C(board.SCL,board.SDA)
nCard = notecard.OpenI2C(port,0,0)
print("Notecard setup complete")
req = {"req":"note.add"}
req["body"] = {"temp": 20.0}

rsp = nCard.Transaction(req)
print(rsp)