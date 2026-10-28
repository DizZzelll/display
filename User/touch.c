#include "main.h"

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "queue.h"
#include "semphr.h"

/*
Сенсор       STM32 Pin       Конфигурация (динамически меняется)
---------------------------------------------------------------
XR (40)  --> PI4             | ЦИФР.ВЫХОД HIGH | АНАЛОГ.ВХОД |
YD (41)  --> PI5             | ЦИФР.ВЫХОД LOW  | АНАЛОГ.ВХОД |
XL (42)  --> PI6             | Hi-Z/АНАЛОГ.ВХОД| ЦИФР.ВЫХОД LOW |
YU (43)  --> PI7             | ЦИФР.ВЫХОД HIGH | Hi-Z/АНАЛОГ.ВХОД |
*/


void touch_task_func(void *argument){
    while(1){
        vTaskDelay(100);
    }
}