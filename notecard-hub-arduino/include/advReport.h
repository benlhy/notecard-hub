#ifndef ADV_REPORT_H
#define ADV_REPORT_H

#include <bluefruit.h>

class AdvReport{
  public:
    uint8_t mac[6];
    int16_t temperature;
    uint16_t humidity;
    uint16_t battery_mv;
    uint8_t battery_percentage;
    uint8_t measurement_count;
    uint8_t flags;

    void processAdvData(ble_gap_evt_adv_report_t* report);
};

#endif