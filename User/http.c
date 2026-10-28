
#include "lwip/apps/httpd.h"
#include "lwip/tcp.h"
#include <string.h>
#include "user.h"


#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "queue.h"
#include "semphr.h"

extern osThreadId_t HttpTaskHandle;

void Notify_HttpTask(void){
    xTaskNotify(HttpTaskHandle, 1, eSetValueWithOverwrite);
}

void HttpTask_func(void *argument){
    
    uint32_t notification;
    xTaskNotifyWait(0, 0, &notification, portMAX_DELAY);
    httpd_init();
    
    while(1){
        osDelay(100);
    }
}

