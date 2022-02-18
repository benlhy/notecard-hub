#include <Arduino.h>
#include <bluefruit.h>
#include <Notecard.h>
#include <Wire.h>
#include "advReport.h"
#include <iterator> // required for std::begin()


#define NOTECARD_ENABLED
#define ProductID "com.gmail.limhanyangb:start"
#define MAX_BLE_REPORTS 20

// Function Prototypes
void scan_callback(ble_gap_evt_adv_report_t* report);
bool match_GAP_type(ble_gap_evt_adv_report_t* report, uint16_t GAP_type);
int add_to_list(AdvReport *array[], int array_len, AdvReport *newReport); 

Notecard notecard;  
AdvReport *scan_array[MAX_BLE_REPORTS];

//0000xxxx-0000-1000-8000-00805F9B34FB

const uint8_t ENV_UUID_SERVICE[] =
{
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x1A, 0x18, 0x00, 0x00
};

const uint8_t ENV_UUID_16_SERVICE[] = {
  0x1A, 0x18
};

int gbl_array_num = 0;

// Process the custom advertising report so that we don't have to connect to the device



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial) delay(10);
  Serial.println("Beginning Scan!");
  
  // Setup Notecard in I2C mode
  Wire.begin();
  #ifdef NOTECARD_ENABLED
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
    
    AdvReport BLEReport;
    BLEReport.processAdvData(report);

    Serial.printf("Temperature: %d\r\n",BLEReport.temperature);
    Serial.printBufferReverse(BLEReport.mac,6,':');

    if(add_to_list(scan_array,gbl_array_num, &BLEReport)) {
      gbl_array_num++; // increment the array size
    }

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



// Check for similar mac addresses
int add_to_list(AdvReport *array[], int array_len, AdvReport *newReport) {
  int count = 0;
  // loop will not run if array len == 0 because condition is false
  for (int i = 0; i<array_len;i++) {
    if( memcmp(array[i]->mac, newReport->mac, 6)) {
      // if we have the same mac, ignore
      count++;
    }
  }
  // count == array_len
  if (count!=array_len) {
    array[array_len+1] = newReport;
    return 1;
  } else {
    return 0;
  }

}

void loop() {

  // BLE scan for and get temp/hum

  // LoRa communication

  #ifdef NOTECARD_ENABLED

  // Check if it is time to send a request

  // Stop the bluetooth scan
  Bluefruit.Scanner.stop();

  // Create a notecard request
  J *req = notecard.newRequest("note.add");
  if (req != NULL) {
    J *body = JCreateObject();
    if (body != NULL) {
      // using the iterator using the non-member begin and end function
      for (auto scan_report = std::begin(scan_array); scan_report != std::end(scan_array); ++scan_report) {
        J *bodyObject = JCreateObject();
        if (bodyObject != NULL) {
          // need to deference the iterator THEN the pointer
          JAddStringToObject(bodyObject,"mac",(const char*)(*scan_report)->mac);
          JAddNumberToObject(bodyObject,"temp",(*scan_report)->temperature);
          JAddNumberToObject(bodyObject,"hum",(*scan_report)->humidity);
          JAddNumberToObject(bodyObject,"batt_p",(*scan_report)->battery_percentage);
          JAddNumberToObject(bodyObject,"batt_v",(*scan_report)->battery_percentage);
        }

      }
      //JAddNumberToObject(body, "temp", );
    }
    notecard.sendRequest(req);
  }

  // Request completed sending

  // Clear array but keep the reference to the global array
  // Using the range-based for-loop syntax
  for (auto array_elm : scan_array) {
    delete array_elm;
  }
  gbl_array_num = 0; //reset to 0
  Bluefruit.Scanner.start(0); // start BLE scanner  
  #endif // NOTECARD_ENABLED

}