
#ifndef SSD1306_H
#define SSD1306_H

#include "main.h"
#include "string.h"
#include "stdio.h"
#include <stdarg.h>

// Адрес дисплея (обычно 0x3C или 0x3D)
#define OLED_ADDRESS 0x3C

// Команды для дисплея
#define OLED_COMMAND 0x00
#define OLED_DATA 0x40

// Размеры дисплея
//#define OLED_WIDTH  128
//#define OLED_HEIGHT  64  //физически экран 128 на 32 буфер больше 128 на 64, переключение страниц добавил
//#define OLED_WIDTH  320
//#define OLED_HEIGHT 240
#define OLED_WIDTH  272
#define OLED_HEIGHT 480
#define REAL_WIDTH  OLED_WIDTH       //реальный размер дисплея
#define REAL_HEIGHT OLED_HEIGHT      //реальный размер дисплея
#define FONT_WIDTH 5
#define FONT_HEIGHT 7

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB888_Pixel;

// Структура для работы с дисплеем
typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint8_t buffer[OLED_WIDTH * OLED_HEIGHT / 8]; //один пиксель == один бит
    uint16_t currentX;
    uint16_t currentY;
    int mempage;
} OLED_HandleTypeDef;

extern OLED_HandleTypeDef oled;

// Функция отправки команды
void OLED_WriteCommand(OLED_HandleTypeDef *oled, uint8_t command);
// Функция отправки данных
void OLED_WriteData(OLED_HandleTypeDef *oled, uint8_t data);

// Инициализация дисплея
void OLED_Init(OLED_HandleTypeDef *oled, I2C_HandleTypeDef *hi2c);
// Очистка дисплея
void OLED_Clear(OLED_HandleTypeDef *oled);

// Обновление экрана
void OLED_UpdateScreen(OLED_HandleTypeDef *oled);

// Установка позиции курсора
void OLED_SetCursor(OLED_HandleTypeDef *oled, uint16_t x, uint16_t y);

void OLED_ClearCharArea(OLED_HandleTypeDef *oled, uint16_t x, uint16_t y);

// Простой вывод символа (базовый шрифт 5x7)
void OLED_WriteChar(OLED_HandleTypeDef *oled, char ch);

// Вывод строки
void OLED_WriteString(uint8_t update_src, OLED_HandleTypeDef *oled, uint16_t row, uint16_t col, char *str, ...);

void OLED_WriteString_light(uint8_t update_src, OLED_HandleTypeDef *oled, uint16_t row, uint16_t col, char *str);

// Перевернуть экран по горизонтали
void OLED_FlipHorizontal(OLED_HandleTypeDef *oled, uint8_t flip);

// Перевернуть экран по вертикали
void OLED_FlipVertical(OLED_HandleTypeDef *oled, uint8_t flip);

// Установка позиции текста (строка 0-3, столбец 0-25)
void OLED_SetTextCursor(OLED_HandleTypeDef *oled, uint16_t row, uint16_t col);

// Инвертировать цвета дисплея
void OLED_InvertColors(OLED_HandleTypeDef *oled, uint8_t invert);

void Copy_oled_mem(OLED_HandleTypeDef *oled, void* lcd_mem, int mem_rotate);

#endif