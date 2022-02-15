#include <Arduino.h>
#include <bluefruit.h>
#include <Notecard.h>

#define ProductID "com.gmail.limhanyangb:start"

Notecard notecard;

//0000xxxx-0000-1000-8000-00805F9B34FB

const uint8_t ENV_UUID_SERVICE[] =
{
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x1A, 0x18, 0x00, 0x00
};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial) delay(10);

  // Setup Notecard in I2C mode
  Wire.begin();
  notecard.begin();

  // Setup Notecard service request
  J *req = notecard.newRequest("hub.set"); 
  JAddStringToObject(req, "product", ProductID); // set the product ID
  JAddStringToObject(req, "mode", "periodic"); // connect periodically
  JAddNumberToObject(req, "outbound", 2); // check for outgoing data every 2 minutes
  JAddNumberToObject(req, "inbound", 60); // check for downloads once every hour
  notecard.sendRequest(req); // send the request


  // Start Bluetooth in Central mode
  Bluefruit.begin(0,1);
  Bluefruit.setTxPower(4);
  Bluefruit.setName("notecard-hub");
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.start(0);

  // TODO: Add lora init 
}

void scan_callback(ble_gap_evt_adv_report_t* report)
{
  Serial.println("Timestamp Addr              Rssi Data");

  Serial.printf("%09d ", millis());
  
  // MAC is in little endian --> print reverse
  Serial.printBufferReverse(report->peer_addr.addr, 6, ':');
  Serial.print(" ");

  Serial.print(report->rssi);
  Serial.print("  ");

  Serial.printBuffer(report->data.p_data, report->data.len, '-');
  Serial.println();

  // Check if advertising contain BleUart service
  if ( Bluefruit.Scanner.checkReportForUuid(report, ENV_UUID_SERVICE) )
  {
    Serial.println("                       ENV UART service detected");
  }

  Serial.println();

  // For Softdevice v6: after received a report, scanner will be paused
  // We need to call Scanner resume() to continue scanning
  Bluefruit.Scanner.resume();
}

void loop() {
  // put your main code here, to run repeatedly:
}