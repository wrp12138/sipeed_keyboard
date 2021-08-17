#ifndef _SYSTEM_SWITCH_
#define _SYSTEM_SWITCH_

#include "hal_uart.h"
#include <FreeRTOS.h>
#include "semphr.h"
#include "bl702.h"
#include "smk_ble.h"
#include "smk_usb.h"
#include "conn.h"
#include "hal_power.h"
#include "hal_clock.h"



void mode_switch(uint8_t flag);
void light_switch(uint8_t flag);
void brightness_switch(uint8_t flag);


#endif
