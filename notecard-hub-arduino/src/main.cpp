#include <Arduino.h>
#include <bluefruit.h>
#include <Notecard.h>
#include "advReport.h"

//#define NOTECARD_ENABLED
#define ProductID "com.gmail.limhanyangb:start"

// Function Prototypes
void scan_callback(ble_gap_evt_adv_report_t* report);
bool match_GAP_type(ble_gap_evt_adv_report_t* report, uint16_t GAP_type);

Notecard notecard;

//0000xxxx-0000-1000-8000-00805F9B34FB

const uint8_t ENV_UUID_SERVICE[] =
{
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x1A, 0x18, 0x00, 0x00
};

const uint8_t ENV_UUID_16_SERVICE[] = {
  0x1A, 0x18
};

// Process the custom advertising report so that we don't have to connect to the device



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial) delay(10);
  Serial.println("Beginning Scan!");
  
  // Setup Notecard in I2C mode
  #ifdef NOTECARD_ENABLED
  Wire.begin();
  notecard.begin();
  

  // Setup Notecard service request
  J *req = notecard.newRequest("hub.set"); 
  JAddStringToObject(req, "product", ProductID); // set the product ID
  JAddStringToObject(req, "mode", "periodic"); // connect periodically
  JAddNumberToObject(req, "outbound", 2); // check for outgoing data every 2 minutes
  JAddNumberToObject(req, "inbound", 60); // check for downloads once every hour
  notecard.sendRequest(req); // send the request
  #endif


  // Start Bluetooth in Central mode
  Bluefruit.begin(0,1);
  Bluefruit.setTxPower(4);
  Bluefruit.setName("notecard-hub");
  Bluefruit.Scanner.setRxCallback(scan_callback);
  //Bluefruit.Scanner.filterUuid(0x181A);
  Bluefruit.Scanner.start(0);

  // TODO: Add lora init 
}

// Example data 
// Type = 3
// 12-16-1A-18-71-37-33-38-C1-A4-B0-0A-FF-17-98-0B-55-BB-05
// 12 = len (18)
// 16 = service-data 16bit uuid 
// ... = data
// https://www.bluetooth.com/specifications/bluetooth-core-specification/ - CSS, section A, 1.11

bool match_GAP_type(ble_gap_evt_adv_report_t* report, uint16_t type) {
  // GAP access profile type -> NOT IMPLEMENTED 
  // TODO: implement a generic access
  // See ble_gap.h
  
  if (report->data.p_data[1] == type) {
    return true;
  } else {
    return false;
  }
}



void scan_callback(ble_gap_evt_adv_report_t* report)
{

  //Serial.printf("RSSI: %d",report->rssi);
  // seeking for Service data type
  //Serial.printBuffer(report->data.p_data, report->data.len, '-');
  if (report->rssi > -50 && match_GAP_type(report,BLE_GAP_AD_TYPE_SERVICE_DATA) ) {
      AdvReport newReport;
  newReport.processAdvData(report);
  Serial.printf("Temperature: %d\r\n",newReport.temperature);
    Serial.println("Timestamp Addr              Rssi Data");

    Serial.printf("%09d ", millis());
    
    // MAC is in little endian --> print reverse
    Serial.printBufferReverse(report->peer_addr.addr, 6, ':');
    Serial.print(" ");

    Serial.print(report->rssi);
    Serial.print("  ");

    Serial.printf("Data Len: %d \r\n", report->data.len);
    Serial.printf("Report type: %d\r\n", report->type);

    Serial.printBuffer(report->data.p_data, report->data.len, '-');
    Serial.println();

    // Check if advertising contain BleUart service
    if ( Bluefruit.Scanner.checkReportForUuid(report, 0x181A) )
    {
      Serial.println("                       ENV UART service detected");
    } else {
      Serial.println();
    }
  }
  // For Softdevice v6: after received a report, scanner will be paused
  // We need to call Scanner resume() to continue scanning
  Bluefruit.Scanner.resume();

  


  

  
}

void loop() {

  // BLE scan for and get temp/hum

  // LoRa communication

  #ifdef NOTECARD_ENABLED
  J *req = notecard.newRequest("note.add");
  if (req != NULL) {
    J *body = JCreateObject()
    if (body != NULL) {
      JAddNumberToObject(body, "temp", 24);
    }
  }
  #endif

  // put your main code here, to run repeatedly:
}