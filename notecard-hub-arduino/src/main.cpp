#include <Arduino.h>
#include <bluefruit.h>
#include <Notecard.h>
#include <Wire.h>
#include "advReport.h"
#include <iterator> // required for std::begin()
#include <SPI.h>
#include <LoRa.h>

//#define NOTECARD_WIFI
//#define NOTECARD_ENABLED
//#define LORA_ENABLED
#define ProductID "com.gmail.limhanyangb:start"
#define SSID "JBW"
#define PASSWORD "130114042809janbenwin"


#define MAX_BLE_REPORTS 20

// Function Prototypes
void scan_callback(ble_gap_evt_adv_report_t* report);
bool match_GAP_type(ble_gap_evt_adv_report_t* report, uint16_t GAP_type);
int add_to_list(AdvReport *array[], int array_len, AdvReport *newReport); 

Notecard notecard;  
auto counter = 0;
AdvReport *scan_array[MAX_BLE_REPORTS];

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
  // setup notecard
  notecard.begin();
  notecard.setDebugOutputStream(Serial);

  #ifdef NOTECARD_WIFI
  J *req = notecard.newRequest("card.wifi");
  JAddStringToObject(req, "ssid", SSID);
  JAddStringToObject(req, "password", PASSWORD);
  notecard.sendRequest(req);  


  #endif

  // Setup Notecard service request
  J *req = notecard.newRequest("hub.set"); 
  JAddStringToObject(req, "product", ProductID);  // set the product ID
  JAddStringToObject(req, "mode", "continuous");
  //JAddStringToObject(req, "mode", "periodic");    // connect periodically
  //JAddNumberToObject(req, "outbound", 3);         // check for outgoing data every 15 minutes
  //JAddNumberToObject(req, "inbound", 60);         // check for downloads once every hour
  notecard.sendRequest(req);                      // send the request
  #endif

  #ifdef LORA_ENABLED
  if(!LoRa.begin(915E6)){
    Serial.println("Lora init failed");

  }

  #endif


  // Start Bluetooth in Central mode
  Bluefruit.begin(0,1);
  Bluefruit.setTxPower(4);
  Bluefruit.setName("notecard-hub");
  Bluefruit.Scanner.setRxCallback(scan_callback);
  //Bluefruit.Scanner.filterUuid(0x181A);
  Bluefruit.Scanner.start(0);

 
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
      Serial.println("Added to array");
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
  if (count==array_len) {
    array[array_len+1] = newReport;
    return 1;
  } else {
    return 0;
  }

}

void loop() {

  // BLE scan for and get temp/hum - covered in  scan
  //Serial.print("In loop");

  // LoRa communication
  #ifdef LORA_ENABLED
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }

      // TODO: add to scan_array

        
  #endif // LORA_ENABLED

  #ifdef NOTECARD_ENABLED

  // Check if it is time to send a request
  if (gbl_array_num>1) {
    // Stop the bluetooth scan
    Serial.println("Stopping BLE scan");
    //Bluefruit.Scanner.stop();
    
    
    // Create a notecard request
    J *req = notecard.newRequest("note.add");
    JAddBoolToObject(req, "sync", true);

    if (req != NULL) {
      J *body = JCreateObject();
      if (body != NULL) {
        // using the iterator using the non-member begin and end function
        // for each element in scan_array
        for (auto scan_report = std::begin(scan_array); scan_report != std::end(scan_array); ++scan_report) {
          J *bodyObject = JCreateObject();
          if (bodyObject != NULL) {
            // need to deference the iterator THEN the pointer
            for(int i = 0;i<6;i++) {
              Serial.println((*scan_report)->mac[i]);

            }
            
            //JAddStringToObject(bodyObject,"mac",(const char*)(*scan_report)->mac);
            //JAddNumberToObject(bodyObject,"temp",(*scan_report)->temperature);
            //JAddNumberToObject(bodyObject,"hum",(*scan_report)->humidity);
            //JAddNumberToObject(bodyObject,"batt_p",(*scan_report)->battery_percentage);
            //JAddNumberToObject(bodyObject,"batt_v",(*scan_report)->battery_percentage);
          }
          JAddItemToObject(body,(const char*)(*scan_report)->mac,bodyObject);
        }
        //JAddNumberToObject(body, "temp", );
      }
      JAddItemToObject(req,"body",body);


      Serial.println("Sending request");
      notecard.sendRequest(req);
      Serial.println("Request send complete");
    }
    // Request completed sending
    // Clear array but keep the reference to the global array
    // Using the range-based for-loop syntax
    for (auto array_elm : scan_array) {
      delete array_elm;
    }
    gbl_array_num = 0; //reset to 0
    Serial.println("Restarting scan...");
    //Bluefruit.Scanner.start(); // start BLE scanner
    Serial.println("Scan restarted.");  
  }
  #endif // NOTECARD_ENABLED

}