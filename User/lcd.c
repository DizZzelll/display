#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "queue.h"
#include "sdram.h"
#include "lcd.h"
#include <string.h>
#include "../Component/hx8257.h"
#include "../Component/hx8257.c"
#include "../Component/ssd1306.h"
#include "../Component/ssd1306.c"
#include <math.h>

#include "button.h"
#include "version.h"

extern osMessageQueueId_t adc_queueHandle;
extern osMessageQueueId_t input_dataHandle;

void init_lcd(void){
    //инициализация SPI в CubeMX
    //инициализация LTDC в CubeMX
    //инициализация external RAM в CubeMX
    HX8257_Init();

    //while(1){
    //    HX8257_WriteReg(0x1, 0xAA);
    //    osDelay(70);
    //    volatile uint16_t data = HX8257_ReadReg(0x1);
    //    printf("%d",data);
   //     osDelay(1000);
    //}
    
}

void print_lcd(int x, int y, const char *format, ...){
    char buff[50] = {0};

    va_list args;
    va_start(args, format);
    vsnprintf(buff, sizeof(buff), format, args);
    va_end(args);

    OLED_WriteString_light(0, &oled, y, x, buff);
}

void upd_lcd(void){
    Copy_oled_mem(&oled,(void*)LCD_FRAME_ADDRESS_SDRAM,-1);
}

void lcd_task_func(void *argument){
    init_lcd();
    
    memset((void*)LCD_FRAME_ADDRESS_SDRAM, 255, LCD_BUFFER_SIZE);
    signed char color = 0;
    Buttons btn;

    while(1){
        osDelay(1);
        print_lcd(1, 0, "fw ver: %s", version);
        print_lcd(1, 1, "counter: %4d", color++);
        //xQueueReceive(adc_queueHandle, &result, 0);
        xQueueReceive(input_dataHandle, &btn.bvalue, 0);
        print_lcd(1, 10, "button up:     %d",btn.bbits.up);
        print_lcd(1, 11, "button down:   %d",btn.bbits.down);
        print_lcd(1, 12, "button left:   %d",btn.bbits.left);
        print_lcd(1, 13, "button right:  %d",btn.bbits.right);
        print_lcd(1, 14, "button enter:  %d",btn.bbits.enter);
        print_lcd(1, 15, "button escape: %d",btn.bbits.escape);
        //print_lcd(0, 1, "stm vdda: %.2f", result);
        //print_lcd(0, 2, "0123456789012345678901234567890123456789012a");
        //print_lcd(0, 5, "0123456789012345678901234567890ab");
        //print_lcd(1, 3, " ", result);
        //print_lcd(0, 4, "%.25f", acos(-1.0));
        upd_lcd();
        //memset((void*)LCD_FRAME_ADDRESS_SDRAM, color, LCD_BUFFER_SIZE);
    }
}
