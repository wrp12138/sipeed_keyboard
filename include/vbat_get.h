#ifndef __vbat_get
#define __vbat_get

#include "hal_adc.h"
#include <FreeRTOS.h>
#include "semphr.h"
#include "bl702.h"
#include "smk_ble.h"
#include "smk_usb.h"
#include <system_switch.h>


struct device * vbat_get_init(void);

uint8_t vbat_get(struct device *adc_vbat_get);


#endif