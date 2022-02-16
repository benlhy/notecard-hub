#include "advReport.h"


void AdvReport::processAdvData(ble_gap_evt_adv_report_t* report) {
  for (int i=0;i<6;i++) {
    mac[i] = report->data.p_data[9-i];
  }
  temperature = (int16_t)(report->data.p_data[11] << 8 | report->data.p_data[10]);
  humidity = (uint16_t)(report->data.p_data[13] << 8 | report->data.p_data[12]);
  battery_mv = (uint16_t)(report->data.p_data[15] << 8 | report->data.p_data[14]);
  battery_percentage = report->data.p_data[16];
  measurement_count = report->data.p_data[17];
  flags = report->data.p_data[18];
}