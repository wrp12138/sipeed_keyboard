#include "vbat_get.h"










struct device * vbat_get_init(void)
{
    adc_channel_t posChList[] = { ADC_CHANNEL4 };
    adc_channel_t negChList[] = { ADC_CHANNEL_GND };

    struct device *adc_vbat_get;
    adc_channel_cfg_t adc_channel_cfg;
    adc_channel_cfg.pos_channel = posChList;
    adc_channel_cfg.neg_channel = negChList;


    adc_register(ADC0_INDEX, "adc_vbat_get");
    adc_vbat_get = device_find("adc_vbat_get");

    if(adc_vbat_get)
    {
        ADC_DEV(adc_vbat_get)->continuous_conv_mode = ENABLE;
        device_open(adc_vbat_get, DEVICE_OFLAG_STREAM_RX);
        device_control(adc_vbat_get,DEVICE_CTRL_ADC_CHANNEL_CONFIG,&adc_channel_cfg);
        adc_channel_start(adc_vbat_get);

        return adc_vbat_get;

    }else{
        MSG("device open failed\r\n");
        return NULL;
    }

    

}

uint8_t vbat_get(struct device *adc_vbat_get)//low vbat_value@3.65v,1825     high vbat_value@4.2v,2100
{
    adc_channel_val_t result_val;
    uint32_t vbat_value;
    uint8_t per;
    if (adc_vbat_get)
    {
        
        device_read(adc_vbat_get, 0, (void *)&result_val, 1);
        vbat_value=result_val.volt * 1000;
        if(vbat_value>2100)
        {
            vbat_value=2100;
        }
        else if(vbat_value<1825)
        {
            vbat_value=1825;
        }
        per=((vbat_value-1825)*100)/(2100-1825);
        return per;
    }
    else
    {
        return 0;
    }
}
