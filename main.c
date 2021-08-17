/**
 * @file main.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "hal_uart.h"
#include <FreeRTOS.h>
#include "semphr.h"
#include "bl702.h"
#include "smk_ble.h"
#include "smk_usb.h"
#include <system_switch.h>
#include <timers.h>
#include "vbat_get.h"
#include "hal_gpio.h"











extern uint8_t _heap_start;
extern uint8_t _heap_size; // @suppress("Type cannot be resolved")
extern uint8_t mode_flag;//ble and usb switch flag
extern uint8_t last_mode_flag;







void hbn_mode(int sleep_time)
{
	lp_set_clock_gate(ENABLE, CLOCK_AHB_ALL);
    lp_set_all_gpio_hz();
    lp_enter_hbn(sleep_time);
}




static HeapRegion_t xHeapRegions[] = {
    { &_heap_start, (unsigned int)&_heap_size },
    { NULL, 0 }, /* Terminates the array. */
    { NULL, 0 }  /* Terminates the array. */
};

uint8_t sharedBuf[16];
void user_vAssertCalled(void) __attribute__((weak, alias("vAssertCalled")));
void vAssertCalled(void)
{
    MSG("vAssertCalled\r\n");

    while (1)
        ;
}

void vApplicationTickHook(void)
{
    //MSG("vApplicationTickHook\r\n");
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    MSG("vApplicationStackOverflowHook\r\n");

    if (pcTaskName) {
        MSG("Stack name %s\r\n", pcTaskName);
    }

    while (1)
        ;
}

void vApplicationMallocFailedHook(void)
{
    MSG("vApplicationMallocFailedHook\r\n");

    while (1)
        ;
}
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

static void ble_init_task(void *pvParameters)
{
    ble_init();
    vTaskDelete(NULL);
}

static void usb_init_task(void *pvParameters) //FIXME
{
    usb_init();
    vTaskDelete(NULL);
}



/*void system_switch_task(void *pvParameters)
{
	while(1)
	{
		if(last_mode_flag!=mode_flag)
		{
			last_mode_flag=mode_flag;
			mode_switch(mode_flag);
		}
		vTaskDelay(1000);
	}
	vTaskDelete(NULL);

}*/



void lowpower_switch(TimerHandle_t xTimer)
{

	hbn_mode(0);
}












int main(void)
{
	gpio_set_mode(GPIO_PIN_9, GPIO_OUTPUT_PP_MODE);

    static StackType_t ble_init_stack[1024];
    static StaticTask_t ble_init_task_h;
    static StackType_t usb_init_stack[512];
    static StaticTask_t usb_init_task_h;
    struct device* vbat_adc;
    //TaskHandle_t system_switch_task_Handler;
    TimerHandle_t lowpower_switch_handle;
    BaseType_t lowpower_switch_timer_start;

    bflb_platform_init(0);

    MSG("Sipeed Machine Keyboard start...\r\n");
    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL);

    vPortDefineHeapRegions(xHeapRegions);


    vbat_adc=vbat_get_init();




    MSG("[SMK] Device init...\r\n");
    //xTaskCreateStatic(ble_init_task, (char *)"ble_init", sizeof(ble_init_stack) / 4, NULL, 14, ble_init_stack, &ble_init_task_h);
    //xTaskCreateStatic(usb_init_task, (char *)"usb_init", sizeof(usb_init_stack) / 4, NULL, 14, usb_init_stack, &usb_init_task_h);
    /*xTaskCreate((TaskFunction_t )system_switch_task,
				 (const char* )"system_switch",
				 (uint16_t )50,
				 (void* )NULL,
				 (UBaseType_t )14,
				 (TaskHandle_t* )&system_switch_task_Handler);*/




    lowpower_switch_handle=xTimerCreate(   (const char *)"lowpower_switch_t",
                                            (TickType_t)5000,
                                            (UBaseType_t)pdTRUE,
                                            (void *) 2,
                                            (TimerCallbackFunction_t)lowpower_switch);





    MSG("[SMK] Start task scheduler...\r\n");
    lowpower_switch_timer_start=xTimerStart(lowpower_switch_handle,0);

    if(lowpower_switch_timer_start==pdPASS)
    {
        vbat_get(vbat_adc);
    	gpio_write(GPIO_PIN_9, 0);
        vTaskStartScheduler();
    }
    

    //hbn_mode(0);
    BL_CASE_SUCCESS;

    while (1) {

        gpio_write(GPIO_PIN_9, 0);
        bflb_platform_delay_ms(100);
    }
}
