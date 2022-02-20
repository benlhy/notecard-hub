#ifndef ADV_REPORT_H
#define ADV_REPORT_H

#include <bluefruit.h>


//0000xxxx-0000-1000-8000-00805F9B34FB

const uint8_t ENV_UUID_SERVICE[] =
{
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x1A, 0x18, 0x00, 0x00
};

const uint8_t ENV_UUID_16_SERVICE[] = {
  0x1A, 0x18
};

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
    char generateMacString(void);
};

#endif