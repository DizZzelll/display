#ifndef LCD_H
#define LCD_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_FRAME_ADDRESS_STATIC_RAM 0x2000A800

#define LCD_COLOR1    1
#define LCD_COLOR565 16
#define LCD_COLOR888 24
#define LCD_BUFFER_SIZE (480 * 272 * LCD_COLOR888 / 8)

/*
128 *  32 *  1bit(monochrome) / 8bit = 512    bytes
128 *  64 *  1bit(monochrome) / 8bit = 1024   bytes
320 * 240 * 16bit(RGB565)     / 8bit = 153600 bytes
320 * 240 * 24bit(RGB888)     / 8bit = 230400 bytes
*/

/* ---- базовый LCD API ---- */
void init_lcd(void);
void print_lcd(int x, int y, const char *format, ...);
void upd_lcd(void);

/* ---- экранное меню / диалоги / ввод (реализация в pd78.c) ---- */

typedef void (*menu_cb)(void);
typedef void (*menu_cb_data)(void*);

void ncmenu_init(void);
void ncmenu_deinit(void);

void ncmenu(const char* title, const char** const choices,
            const menu_cb* cb, size_t count);
void ncmenu_data(const char* title, const char** const choices,
                 const menu_cb_data* cb, void** const data, size_t count);

int  ncdialog_yesno(const char* title, const char* message, ...);
void ncdialog_info   (const char* title, const char* message, ...);
void ncdialog_warning(const char* title, const char* message, ...);
void ncdialog_error  (const char* title, const char* message, ...);

int ncinput_int     (const char* title, int* number, int minimum, int maximum);
int ncinput_uint    (const char* title, unsigned int* number, unsigned int maximum);
int ncinput_uint_min(const char* title, unsigned int* number,
                     unsigned int minimum, unsigned int maximum);

#ifdef __cplusplus
}
#endif
#endif /* LCD_H */
