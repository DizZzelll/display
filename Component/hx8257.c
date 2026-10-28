#include "hx8257.h"
#include "main.h"

// Внутренняя функция записи через SPI
static void HX8257_SPI_Write(uint8_t* data, uint16_t size) {
    HX8257_CSB_LOW();
    HAL_SPI_Transmit(&hspi2, data, size, HAL_MAX_DELAY);
    HX8257_CSB_HIGH();
    DWT_Delay(0.000001);
}

// Запись регистра (24-бит SPI, как в даташите)
void HX8257_WriteReg(uint8_t reg, uint16_t data) {
    uint8_t spiData[3];

    // Первая передача: адрес регистра
    spiData[0] = 0x70;           // RS=0, RW=0 (запись)
    spiData[1] = 0x00;            // адрес регистра
    spiData[2] = reg;
    HX8257_SPI_Write(spiData, 3);

    // Вторая передача: данные
    spiData[0] = 0x72;           // RS=1, RW=0 (запись данных)
    spiData[1] = (data >> 8) & 0xFF; // старший байт
    spiData[2] = data & 0xFF;        // младший байт
    HX8257_SPI_Write(spiData, 3);
}

// Функция для чтения регистра
uint16_t HX8257_ReadReg(uint16_t reg) {
    uint8_t tx0[3] = {0x70, reg >> 8, reg};  // RS=0 RW=0 Write SPI Address
    uint8_t tx1[3] = {0x73, 0x00, 0x00};     // RS=1 RW=1 Read SPI Data
    uint8_t rx1[3] = {0};
    
    HX8257_CSB_LOW();
    HAL_SPI_Transmit(&hspi2, tx0, sizeof(tx0), HAL_MAX_DELAY);
    HX8257_CSB_HIGH();
    DWT_Delay(0.000001);
    HX8257_CSB_LOW();
    HAL_SPI_TransmitReceive(&hspi2, tx1, rx1, 3, HAL_MAX_DELAY);
    HX8257_CSB_HIGH();
    
    return (rx1[1] << 8) | rx1[2];
}

// Аппаратный сброс
void HX8257_Reset(void) {
    HX8257_RESETB_LOW();
    HAL_Delay(10); // минимум 1 мс по даташиту
    HX8257_RESETB_HIGH();
    HAL_Delay(10);
}

// Включение дисплея
void HX8257_DisplayOn(void) {
    HX8257_DISP_ON();
}

// Выключение дисплея
void HX8257_DisplayOff(void) {
    HX8257_DISP_OFF();
}

// Основная инициализация HX8257
void HX8257_Init(void) {
    // 1. Настройка пинов
    //GPIO_InitTypeDef GPIO_InitStruct = {0};

    //__HAL_RCC_GPIOA_CLK_ENABLE(); // Включить тактирование порта

    // CSB
    //GPIO_InitStruct.Pin = HX8257_CSB_PIN;
    //GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    //GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    //HAL_GPIO_Init(HX8257_CSB_PORT, &GPIO_InitStruct);
    HX8257_CSB_HIGH();

    // RESETB
    //GPIO_InitStruct.Pin = HX8257_RESETB_PIN;
    //HAL_GPIO_Init(HX8257_RESETB_PORT, &GPIO_InitStruct);
    HX8257_RESETB_HIGH();

    // DISP
    //GPIO_InitStruct.Pin = HX8257_DISP_PIN;
    //HAL_GPIO_Init(HX8257_DISP_PORT, &GPIO_InitStruct);
    HX8257_DISP_ON();
    return;


    // 2. Сброс
    HX8257_Reset();
    
    // 3. Инициализация регистров (примерная настройка под 480x272)
    HAL_Delay(100);

    // Driver Data Control (R01h)
    // LR=1, UD=1, NBW=1 (нормально черный), BGR=0, включен дизеринг
    HX8257_WriteReg(REG_DRIVER_DATA_CONTROL, 0x1C00); 

    // Power Control 1 (R02h)
    // Пример: VDS=00, EQ=010, DC=0000, AP=000
    HX8257_WriteReg(REG_POWER_CONTROL_1, 0x0CC0);

    // Power Control 2 (R03h)
    // Включение внутренних преобразователей
    HX8257_WriteReg(REG_POWER_CONTROL_2, 0x000F);

    // Function Control (R04h)
    // Вариант 1: Явное указание всех битов
    // PS=1, PSHUT=1, Bit8=1 (0x0100)
    // (1 << 4) | (1 << 3) | 0x0100 = 0x0118
    HX8257_WriteReg(REG_FUNCTION_CONTROL, 0x0118);

    // VCOM Control (R06h)
    // Настройка VCOM (пример)
    HX8257_WriteReg(REG_VCOM_CONTROL, 0x1C1C);

    // Vertical Porch (R07h)
    HX8257_WriteReg(REG_VERTICAL_PORCH, 0x0A00); // подстройте под ваши тайминги

    // Horizontal Porch (R08h)
    HX8257_WriteReg(REG_HORIZONTAL_PORCH, 0x2D00); // подстройте

    // Контраст/Яркость (R09h)
    HX8257_WriteReg(REG_CONTRAST_BRIGHTNESS, 0x8080);

    // Gamma настройки (пример)
    HX8257_WriteReg(REG_GAMMA_CONTROL_1, 0x003F);  // Positive Gamma Correction
    HX8257_WriteReg(REG_GAMMA_CONTROL_2, 0x0022);
    HX8257_WriteReg(REG_GAMMA_CONTROL_3, 0x0036);
    HX8257_WriteReg(REG_GAMMA_CONTROL_4, 0x0049);
    HX8257_WriteReg(REG_GAMMA_CONTROL_5, 0x00F6);
    HX8257_WriteReg(REG_GAMMA_CONTROL_6, 0x0019);
    HX8257_WriteReg(REG_GAMMA_CONTROL_7, 0x001B);
    HX8257_WriteReg(REG_GAMMA_CONTROL_8, 0x00F8);
    HX8257_WriteReg(REG_GAMMA_CONTROL_9, 0x0049);
    HX8257_WriteReg(REG_GAMMA_CONTROL_10, 0x000F);
    // ... остальные gamma-регистры

    // 4. Включение дисплея
    HAL_Delay(10);
    HX8257_DisplayOn();
}