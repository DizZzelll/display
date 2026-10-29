#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "queue.h"
#include "sdram.h"
#include "lcd.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "../Component/hx8257.h"
#include "../Component/ssd1306.h"
#include "button.h"
#include "version.h"

/* ---- fallback, если в version.h нет VERSION_STRING ---- */
#ifndef VERSION_STRING
  #ifdef FW_VERSION
    #define VERSION_STRING FW_VERSION
  #elif defined(VERSION)
    #define VERSION_STRING VERSION
  #else
    #define VERSION_STRING "1.0"
  #endif
#endif

extern osMessageQueueId_t adc_queueHandle;
extern osMessageQueueId_t input_dataHandle;

/* ===================== БАЗОВЫЙ LCD ===================== */
void init_lcd(void) {
    HX8257_Init();
}

void print_lcd(int x, int y, const char *format, ...) {
    char buff[50] = {0};
    va_list args;
    va_start(args, format);
    vsnprintf(buff, sizeof(buff), format, args);
    va_end(args);
    OLED_WriteString_light(0, &oled, y, x, buff);
}

void upd_lcd(void) {
    Copy_oled_mem(&oled, (void*)LCD_FRAME_ADDRESS_SDRAM, -1);
}

/* ===================== МЕНЮ ===================== */
#define NC_COLS       21
#define NC_ROWS       4
#define NC_TITLE_ROW  0
#define NC_SEP_ROW    1
#define NC_BODY_ROW   3
#define NC_HINT_ROW   (NC_ROWS - 1)
#define NC_BODY_ROWS  (NC_HINT_ROW - NC_BODY_ROW)

#define NC_HINT_MENU  "Вверх/Вниз Enter=OK Esc=Назад"
#define NC_HINT_INPUT "Вверх/Вниз изменить  Enter=OK  Esc=Отмена"

typedef enum {
    NC_EV_NONE = 0,
    NC_EV_UP,
    NC_EV_DOWN,
    NC_EV_LEFT,
    NC_EV_RIGHT,
    NC_EV_ENTER,
    NC_EV_ESCAPE
} nc_event_t;

static Buttons nc_prev;

static void nc_clear_line(int y) {
    char s[NC_COLS + 1];
    memset(s, ' ', NC_COLS);
    s[NC_COLS] = 0;
    print_lcd(0, y, "%s", s);
}

static void nc_clear_all(void) {
    for (int i = 0; i < NC_ROWS; i++) nc_clear_line(i);
}

static void nc_draw_title(const char* title) {
    nc_clear_line(NC_TITLE_ROW);
    if (title && *title) print_lcd(1, NC_TITLE_ROW, "%s", title);

    nc_clear_line(NC_SEP_ROW);
    char sep[NC_COLS + 1];
    memset(sep, '-', NC_COLS);
    sep[NC_COLS] = 0;
    print_lcd(0, NC_SEP_ROW, "%s", sep);
}

static void nc_print_wrapped(const char* text, int r0, int r1) {
    const int w = NC_COLS - 2;
    const char* p = text;
    int row = r0;

    while (*p && row <= r1) {
        const char* nl = strchr(p, '\n');
        int avail = nl ? (int)(nl - p) : (int)strlen(p);
        int take = avail > w ? w : avail;
        char tmp[128];

        if (avail > w) {
            for (int i = w - 1; i > 0; --i)
                if (p[i] == ' ') { take = i; break; }
        }
        if (take >= (int)sizeof(tmp)) take = sizeof(tmp) - 1;
        memcpy(tmp, p, take);
        tmp[take] = 0;
        print_lcd(1, row++, "%s", tmp);
        p += take;
        if (*p == ' ' || *p == '\n') p++;
    }
}

static void nc_drain(void) {
    Buttons b;
    int guard = 64;
    while (guard-- > 0 && xQueueReceive(input_dataHandle, &b.bvalue, 0) == pdTRUE)
        nc_prev.bvalue = b.bvalue;
}

static nc_event_t nc_wait(void) {
    Buttons b;
    for (;;) {
        if (xQueueReceive(input_dataHandle, &b.bvalue, portMAX_DELAY) == pdTRUE) {
            if (b.bvalue != nc_prev.bvalue) {
                nc_prev.bvalue = b.bvalue;
                switch (b.bvalue) {
                    case BTN_UP:    return NC_EV_UP;
                    case BTN_DOWN:  return NC_EV_DOWN;
                    case BTN_LEFT:  return NC_EV_LEFT;
                    case BTN_RIGHT: return NC_EV_RIGHT;
                    case BTN_OK:    return NC_EV_ENTER;
                    case BTN_BACK:  return NC_EV_ESCAPE;
                    default: break;
                }
            }
        }
    }
}

static void nc_hint(const char* hint) {
    nc_clear_line(NC_HINT_ROW);
    if (hint) print_lcd(0, NC_HINT_ROW, "%s", hint);
}

/* ---------- Меню с колбэками без данных ---------- */
void ncmenu_init(void) {
    nc_drain();
}

void ncmenu_deinit(void) {
    nc_clear_all();
    upd_lcd();
}

void ncmenu(const char* title, const char** const choices,
            const menu_cb* cb, size_t count) {
    if (!choices || count == 0) return;

    size_t sel = 0;
    nc_drain();
    nc_clear_all();
    nc_draw_title(title);
    nc_hint(NC_HINT_MENU);

    for (;;) {
        for (size_t i = 0; i < count && i < NC_BODY_ROWS; i++) {
            nc_clear_line(NC_BODY_ROW + (int)i);
            if (i == sel)
                print_lcd(0, NC_BODY_ROW + (int)i, "> %s", choices[i]);
            else
                print_lcd(1, NC_BODY_ROW + (int)i, "%s", choices[i]);
        }
        upd_lcd();

        nc_event_t ev = nc_wait();

        switch (ev) {
            case NC_EV_UP:
                if (sel > 0) sel--;
                break;
            case NC_EV_DOWN:
                if (sel + 1 < count) sel++;
                break;
            case NC_EV_ENTER:
                if (cb && cb[sel]) cb[sel]();
                /* после возврата из колбэка — восстановить меню */
                nc_clear_all();
                nc_draw_title(title);
                nc_hint(NC_HINT_MENU);
                break;
            case NC_EV_ESCAPE:
                nc_clear_all();
                return;
            default:
                break;
        }
    }
}

/* ---------- Меню с колбэками, принимающими данные ---------- */
void ncmenu_data(const char* title, const char** const choices,
                 const menu_cb_data* cb, void** const data, size_t count) {
    if (!choices || count == 0) return;

    size_t sel = 0;
    nc_drain();
    nc_clear_all();
    nc_draw_title(title);
    nc_hint(NC_HINT_MENU);

    for (;;) {
        for (size_t i = 0; i < count && i < NC_BODY_ROWS; i++) {
            nc_clear_line(NC_BODY_ROW + (int)i);
            if (i == sel)
                print_lcd(0, NC_BODY_ROW + (int)i, "> %s", choices[i]);
            else
                print_lcd(1, NC_BODY_ROW + (int)i, "%s", choices[i]);
        }
        upd_lcd();

        nc_event_t ev = nc_wait();

        switch (ev) {
            case NC_EV_UP:
                if (sel > 0) sel--;
                break;
            case NC_EV_DOWN:
                if (sel + 1 < count) sel++;
                break;
            case NC_EV_ENTER:
                if (cb && cb[sel]) cb[sel](data ? data[sel] : NULL);
                nc_clear_all();
                nc_draw_title(title);
                nc_hint(NC_HINT_MENU);
                break;
            case NC_EV_ESCAPE:
                nc_clear_all();
                return;
            default:
                break;
        }
    }
}

/* ---------- Диалоги ---------- */
static int nc_dialog(const char* title, const char* message,
                     va_list args, int yesno) {
    char msg[256];
    vsnprintf(msg, sizeof(msg), message, args);

    nc_drain();
    nc_clear_all();
    nc_draw_title(title);
    nc_print_wrapped(msg, NC_BODY_ROW, NC_HINT_ROW - 1);

    if (yesno) {
        nc_hint("Enter=Да  Esc=Нет");
        upd_lcd();
        for (;;) {
            nc_event_t ev = nc_wait();
            if (ev == NC_EV_ENTER)  { nc_clear_all(); upd_lcd(); return 0; }
            if (ev == NC_EV_ESCAPE) { nc_clear_all(); upd_lcd(); return -1; }
        }
    } else {
        nc_hint("Enter=OK");
        upd_lcd();
        for (;;) {
            nc_event_t ev = nc_wait();
            if (ev == NC_EV_ENTER || ev == NC_EV_ESCAPE) {
                nc_clear_all();
                upd_lcd();
                return 0;
            }
        }
    }
}

int ncdialog_yesno(const char* title, const char* message, ...) {
    va_list args;
    va_start(args, message);
    int r = nc_dialog(title, message, args, 1);
    va_end(args);
    return r;
}

void ncdialog_info(const char* title, const char* message, ...) {
    va_list args;
    va_start(args, message);
    nc_dialog(title, message, args, 0);
    va_end(args);
}

void ncdialog_warning(const char* title, const char* message, ...) {
    va_list args;
    va_start(args, message);
    nc_dialog(title, message, args, 0);
    va_end(args);
}

void ncdialog_error(const char* title, const char* message, ...) {
    va_list args;
    va_start(args, message);
    nc_dialog(title, message, args, 0);
    va_end(args);
}

/* ---------- Ввод чисел ---------- */
static int nc_input_uint_range(const char* title, unsigned int* number,
                               unsigned int minimum, unsigned int maximum) {
    unsigned int val = *number;
    if (val < minimum) val = minimum;
    if (val > maximum) val = maximum;

    nc_drain();
    nc_clear_all();
    nc_draw_title(title);
    nc_hint(NC_HINT_INPUT);

    for (;;) {
        nc_clear_line(NC_BODY_ROW);
        print_lcd(1, NC_BODY_ROW, "%u", val);
        upd_lcd();

        nc_event_t ev = nc_wait();
        switch (ev) {
            case NC_EV_UP:
                if (val < maximum) val++;
                break;
            case NC_EV_DOWN:
                if (val > minimum) val--;
                break;
            case NC_EV_ENTER:
                *number = val;
                nc_clear_all();
                upd_lcd();
                return 0;
            case NC_EV_ESCAPE:
                nc_clear_all();
                upd_lcd();
                return -1;
            default:
                break;
        }
    }
}

int ncinput_int(const char* title, int* number, int minimum, int maximum) {
    int val = *number;
    if (val < minimum) val = minimum;
    if (val > maximum) val = maximum;

    nc_drain();
    nc_clear_all();
    nc_draw_title(title);
    nc_hint(NC_HINT_INPUT);

    for (;;) {
        nc_clear_line(NC_BODY_ROW);
        print_lcd(1, NC_BODY_ROW, "%d", val);
        upd_lcd();

        nc_event_t ev = nc_wait();
        switch (ev) {
            case NC_EV_UP:
                if (val < maximum) val++;
                break;
            case NC_EV_DOWN:
                if (val > minimum) val--;
                break;
            case NC_EV_ENTER:
                *number = val;
                nc_clear_all();
                upd_lcd();
                return 0;
            case NC_EV_ESCAPE:
                nc_clear_all();
                upd_lcd();
                return -1;
            default:
                break;
        }
    }
}

int ncinput_uint(const char* title, unsigned int* number, unsigned int maximum) {
    return nc_input_uint_range(title, number, 0u, maximum);
}

int ncinput_uint_min(const char* title, unsigned int* number,
                     unsigned int minimum, unsigned int maximum) {
    return nc_input_uint_range(title, number, minimum, maximum);
}

/* ===================== ДЕМО-ПУНКТЫ МЕНЮ (как в ТЗ) ===================== */

/* 1) Простой обработчик без данных — info-диалог */
static void menuActivation(void)
{
    ncdialog_info("Активизация", "Модуль активирован.\nВерсия: %s",
                  VERSION_STRING);
}

/* 2) yes/no + error, как в ТЗ */
static void menuInit(void)
{
    if (ncdialog_yesno("Загрузка СПО",
                       "Установите SD с ключами СПО в порт SD2") != 0)
    {
        ncdialog_warning("Загрузка СПО", "Действие отменено пользователем");
        return;
    }

    /* В реальном коде здесь было бы device_mount / чтение ключей СПО.
       Для демонстрации — выводим ошибку с подстановкой числа. */
    int result = 5; /* замените на реальный код возврата */
    ncdialog_error("Загрузка СПО", "Ошибка монтирования SD1: %d", result);
}

/* 3) Ввод диапазона через ncinput_uint_min, как в ТЗ */
static void menuRange(void)
{
    unsigned int first = 1;
    unsigned int last  = 10;

    /* В реальном коде: info->hdd.keyCount. Для демо — 100. */
    const unsigned int keyCount = 100;

    if (ncinput_uint_min("Начало диапазона", &first, 1, keyCount) != 0) {
        return;
    }
    if (ncinput_uint_min("Конец диапазона", &last, first, keyCount) != 0) {
        return;
    }

    ncdialog_info("Диапазон", "Выбрано: %u - %u", first, last);
}

/* 4) О программе */
static void menuAbout(void)
{
    ncdialog_info("О программе",
                  "М-711Ш\nВерсия: %s\nВетка: feature/interactive",
                  VERSION_STRING);
}

/* ---------- Пример меню с данными (аналог menuMain2 из ТЗ) ---------- */
static void menuActivationWithData(void* data)
{
    uint32_t* v = (uint32_t*)data;
    if (v) {
        ncdialog_info("Активизация", "Данные: %lu", (unsigned long)*v);
    }
}

static void menuInitWithData(void* data)
{
    uint32_t* v = (uint32_t*)data;
    if (v) {
        ncdialog_info("Инициализация", "Данные: %lu", (unsigned long)*v);
    }
}

void menuMain2(void)
{
    const char* names[] = {
        "Активизация",
        "Инициализация"
    };

    const menu_cb_data cb[] = {
        &menuActivationWithData,
        &menuInitWithData
    };

    uint32_t dataActivation = 15;
    uint32_t dataInit       = 17;

    /* Без const, чтобы не спорить с void** в прототипе ncmenu_data. */
    void* cb_data[] = {
        &dataActivation,
        &dataInit
    };

    ncmenu_data("Главное меню (с данными)", names, cb, cb_data, 2);
}

/* ===================== ГЛАВНОЕ МЕНЮ ===================== */

void menuMain(void)
{
    const char* names[] = {
        "Активизация",
        "Инициализация",
        "Диапазон",
        "С данными",
        "О программе"
    };

    const menu_cb cb[] = {
        &menuActivation,
        &menuInit,
        &menuRange,
        &menuMain2,     /* подменю с данными — обычный колбэк */
        &menuAbout
    };

    ncmenu("Главное меню", names, cb, 5);
}

/* ===================== ЗАДАЧА ЭКРАНА ===================== */

void LCD_task_func(void *argument)
{
    (void)argument;

    /* 1) Инициализация железа и подсистемы меню */
    init_lcd();
    ncmenu_init();

    /* 2) Основной цикл: показываем главное меню. */
    for (;;)
    {
        menuMain();     /* блокирующая, крутится до Esc */
        upd_lcd();
        osDelay(50);    /* пауза, если меню сразу вернулось */
    }

    /* Не достигается, но если кто-то поменяет логику на return: */
    /* ncmenu_deinit(); */
    /* vTaskDelete(NULL); */
}