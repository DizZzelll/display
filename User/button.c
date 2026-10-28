#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "queue.h"

#include "main.h"
#include "button.h"

extern osMessageQueueId_t input_dataHandle;

void button_task_func(void *argument){
     while(1){
        Buttons btn;
        btn.bvalue = (GPIOH->IDR & 0x1F80) >> 7;
        xQueueSend(input_dataHandle, &btn.bvalue, 0);
        osDelay(10);
     }
}