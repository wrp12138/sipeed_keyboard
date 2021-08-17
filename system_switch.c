#include <system_switch.h>
uint8_t mode_flag;//ble and usb switch flag
uint8_t last_mode_flag;

uint8_t power_flag;//power switch flag

uint8_t light_flag;//light switch flag

uint8_t brightness_control;//brightness value,0-255

uint8_t battery_level;//battery level value,0-255

uint8_t key_flag;//key mode flag







void mode_switch(uint8_t flag)
{
	switch(flag){
		//case 0:bt_conn_disconnect(struct bt_conn *conn, u8_t reason);bt_le_adv_stop();usb_init_task();break;//usb mode
		case 1:ble_init();break;//ble mode
	}
}

void light_switch(uint8_t flag)
{
	switch(flag){
		case 0:break;//open light
		case 1:break;//close light
	}
}

void brightness_switch(uint8_t flag)
{

}
