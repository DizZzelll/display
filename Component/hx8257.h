#ifndef HX8257_H
#define HX8257_H

#include "main.h" // или другой серии

// Порт SPI для HX8257
extern SPI_HandleTypeDef hspi2; // укажите ваш SPI

// Пин управления CSB (Chip Select)
#define HX8257_CSB_PORT    GPIOI
#define HX8257_CSB_PIN     GPIO_PIN_1

// Пин RESETB (Reset)
#define HX8257_RESETB_PORT GPIOI
#define HX8257_RESETB_PIN  GPIO_PIN_0

// Пин DISP (Display On/Off)
#define HX8257_DISP_PORT   GPIOH     
#define HX8257_DISP_PIN    GPIO_PIN_15 

// Вспомогательные макросы для управления пинами
#define HX8257_CSB_LOW()   HAL_GPIO_WritePin(HX8257_CSB_PORT, HX8257_CSB_PIN, GPIO_PIN_RESET)
#define HX8257_CSB_HIGH()  HAL_GPIO_WritePin(HX8257_CSB_PORT, HX8257_CSB_PIN, GPIO_PIN_SET)
#define HX8257_RESETB_LOW() HAL_GPIO_WritePin(HX8257_RESETB_PORT, HX8257_RESETB_PIN, GPIO_PIN_RESET)
#define HX8257_RESETB_HIGH() HAL_GPIO_WritePin(HX8257_RESETB_PORT, HX8257_RESETB_PIN, GPIO_PIN_SET)
#define HX8257_DISP_ON()   HAL_GPIO_WritePin(HX8257_DISP_PORT, HX8257_DISP_PIN, GPIO_PIN_SET)
#define HX8257_DISP_OFF()  HAL_GPIO_WritePin(HX8257_DISP_PORT, HX8257_DISP_PIN, GPIO_PIN_RESET)

// Регистры HX8257 (согласно даташиту)
typedef enum {
    REG_DRIVER_DATA_CONTROL = 0x01,
    REG_POWER_CONTROL_1     = 0x02,
    REG_POWER_CONTROL_2     = 0x03,
    REG_FUNCTION_CONTROL    = 0x04,
    REG_PWM_CONTROL         = 0x05,
    REG_VCOM_CONTROL        = 0x06,
    REG_VERTICAL_PORCH      = 0x07,
    REG_HORIZONTAL_PORCH    = 0x08,
    REG_CONTRAST_BRIGHTNESS = 0x09,
    REG_CABC_CONTROL        = 0x0F,
    REG_GAMMA_CONTROL_1     = 0x10,
    REG_GAMMA_CONTROL_2     = 0x11,
    REG_GAMMA_CONTROL_3     = 0x12,
    REG_GAMMA_CONTROL_4     = 0x13,
    REG_GAMMA_CONTROL_5     = 0x14,
    REG_GAMMA_CONTROL_6     = 0x15,
    REG_GAMMA_CONTROL_7     = 0x16,
    REG_GAMMA_CONTROL_8     = 0x17,
    REG_GAMMA_CONTROL_9     = 0x18,
    REG_GAMMA_CONTROL_10     = 0x19,
    // ... остальные регистры
} HX8257_Register;

// Прототипы функций
void HX8257_Init(void);
void HX8257_WriteReg(uint8_t reg, uint16_t data);
uint16_t HX8257_ReadReg(uint16_t reg);
void HX8257_Reset(void);
void HX8257_DisplayOn(void);
void HX8257_DisplayOff(void);

#endif