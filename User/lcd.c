#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "queue.h"
#include "sdram.h"
#include "lcd.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "../Component/hx8257.h"
#include "../Component/hx8257.c"
#include "../Component/ssd1306.h"
#include "../Component/ssd1306.c"
#include <math.h>

#include "button.h"
#include "version.h"

extern osMessageQueueId_t adc_queueHandle;
extern osMessageQueueId_t input_dataHandle;

/* ===================== БАЗОВЫЙ LCD ===================== */

void init_lcd(void){
    HX8257_Init();
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
    Copy_oled_mem(&oled, (void*)LCD_FRAME_ADDRESS_SDRAM, -1);
}

/* ===================== МЕНЮ ===================== */

#define NC_COLS        42
#define NC_ROWS        16
#define NC_TITLE_ROW   0
#define NC_SEP_ROW     1
#define NC_BODY_ROW    2
#define NC_HINT_ROW    (NC_ROWS - 1)
#define NC_BODY_ROWS   (NC_HINT_ROW - NC_BODY_ROW)

typedef enum {
    NC_EV_NONE = 0, NC_EV_UP, NC_EV_DOWN, NC_EV_LEFT, NC_EV_RIGHT,
    NC_EV_ENTER, NC_EV_ESCAPE
} nc_event_t;

static Buttons nc_prev;

static void nc_clear_line(int y){
    char s[NC_COLS + 1];
    memset(s, ' ', NC_COLS); s[NC_COLS] = 0;
    print_lcd(0, y, "%s", s);
}

static void nc_clear_all(void){
    int i;
    for (i = 0; i < NC_ROWS; i++) nc_clear_line(i);
}

static void nc_draw_title(const char* title){
    nc_clear_line(NC_TITLE_ROW);
    if (title && *title) print_lcd(1, NC_TITLE_ROW, "%s", title);
    nc_clear_line(NC_SEP_ROW);
    {
        char sep[NC_COLS + 1];
        memset(sep, '-', NC_COLS); sep[NC_COLS] = 0;
        print_lcd(0, NC_SEP_ROW, "%s", sep);
    }
}

static void nc_print_wrapped(const char* text, int r0, int r1){
    const int w = NC_COLS - 2;
    const char* p = text;
    int row = r0;
    while (*p && row <= r1){
        const char* nl = strchr(p, '\n');
        int avail = nl ? (int)(nl - p) : (int)strlen(p);
        int take = avail > w ? w : avail;
        char tmp[128];
        if (avail > w){
            int i;
            for (i = w - 1; i > 0; --i)
                if (p[i] == ' '){ take = i; break; }
        }
        if (take >= (int)sizeof(tmp)) take = sizeof(tmp) - 1;
        memcpy(tmp, p, take); tmp[take] = 0;
        print_lcd(1, row++, "%s", tmp);
        p += take;
        if (*p == ' ' || *p == '\n') p++;
    }
}

static void nc_drain(void){
    Buttons b;
    int guard = 64;
    while (guard-- > 0 && xQueueReceive(input_dataHandle, &b.bvalue, 0) == pdTRUE)
        nc_prev.bvalue = b.bvalue;
}

static nc_event_t nc_wait(void){
    Buttons b;
    for (;;){
        if (xQueueReceive(input_dataHandle, &b.bvalue, portMAX_DELAY) != pdTRUE)
            continue;
        {
            Buttons edge;
            edge.bvalue = b.bvalue & ~nc_prev.bvalue;
            nc_prev.bvalue = b.bvalue;
            if (edge.bbits.enter)  return NC_EV_ENTER;
            if (edge.bbits.escape) return NC_EV_ESCAPE;
            if (edge.bbits.up)     return NC_EV_UP;
            if (edge.bbits.down)   return NC_EV_DOWN;
            if (edge.bbits.left)   return NC_EV_LEFT;
            if (edge.bbits.right)  return NC_EV_RIGHT;
        }
    }
}

void ncmenu_init(void)  { nc_prev.bvalue = 0; }
void ncmenu_deinit(void){ nc_clear_all(); upd_lcd(); }

static int nc_run(const char* title, const char** ch, size_t n,
                  const menu_cb* cb, const menu_cb_data* cbd,
                  void** data, int use_data)
{
    size_t sel, scroll, i;
    if (!title || !ch || n == 0) return -1;
    nc_drain();
    sel = 0; scroll = 0;

    for (;;){
        nc_clear_all();
        nc_draw_title(title);

        if (sel < scroll) scroll = sel;
        if (sel >= scroll + NC_BODY_ROWS) scroll = sel - NC_BODY_ROWS + 1;

        for (i = 0; i < NC_BODY_ROWS && (i + scroll) < n; i++){
            size_t k = i + scroll;
            char line[NC_COLS + 1];
            snprintf(line, sizeof(line), "%c %s",
                     (k == sel) ? '>' : ' ', ch[k]);
            line[NC_COLS] = 0;
            print_lcd(0, NC_BODY_ROW + (int)i, "%s", line);
        }

        if (n > NC_BODY_ROWS)
            print_lcd(0, NC_HINT_ROW, "Up/Down Enter=OK Esc=Back (%u/%u)",
                      (unsigned)(sel + 1), (unsigned)n);
        else
            print_lcd(0, NC_HINT_ROW, "Up/Down Enter=OK Esc=Back");

        upd_lcd();

        switch (nc_wait()){
        case NC_EV_UP:   if (sel > 0)   sel--; break;
        case NC_EV_DOWN: if (sel+1 < n) sel++; break;
        case NC_EV_ENTER: {
            int has;
            nc_clear_all(); upd_lcd();
            has = use_data ? (cbd && cbd[sel]) : (cb && cb[sel]);
            if (!has){ nc_drain(); return -1; }
            if (use_data) cbd[sel](data ? data[sel] : NULL);
            else          cb[sel]();
            nc_drain();
            return 0;
        }
        case NC_EV_ESCAPE:
            nc_clear_all(); upd_lcd();
            return -1;
        default: break;
        }
    }
}

void ncmenu(const char* t, const char** const c, const menu_cb* cb, size_t n){
    nc_run(t, c, n, cb, NULL, NULL, 0);
}
void ncmenu_data(const char* t, const char** const c,
                 const menu_cb_data* cb, void** const d, size_t n){
    nc_run(t, c, n, NULL, cb, d, 1);
}

static int nc_dialog(const char* title, const char* msg, va_list ap, int yesno){
    char buf[256];
    vsnprintf(buf, sizeof(buf), msg, ap);
    nc_drain();
    for (;;){
        nc_clear_all();
        nc_draw_title(title);
        nc_print_wrapped(buf, NC_BODY_ROW, NC_HINT_ROW - 1);
        print_lcd(0, NC_HINT_ROW, yesno ? "[Enter] Yes  [Esc] No" : "[Enter] OK");
        upd_lcd();
        switch (nc_wait()){
        case NC_EV_ENTER:  nc_clear_all(); upd_lcd(); return yesno ? 0 : 0;
        case NC_EV_ESCAPE: nc_clear_all(); upd_lcd(); return yesno ? 1 : 0;
        default: break;
        }
    }
}

int  ncdialog_yesno(const char* t, const char* m, ...){ va_list a; va_start(a,m); int r=nc_dialog(t,m,a,1); va_end(a); return r; }
void ncdialog_info   (const char* t, const char* m, ...){ va_list a; va_start(a,m); nc_dialog(t,m,a,0); va_end(a); }
void ncdialog_warning(const char* t, const char* m, ...){ va_list a; va_start(a,m); nc_dialog(t,m,a,0); va_end(a); }
void ncdialog_error  (const char* t, const char* m, ...){ va_list a; va_start(a,m); nc_dialog(t,m,a,0); va_end(a); }

int ncinput_uint_min(const char* title, unsigned int* num,
                     unsigned int mn, unsigned int mx)
{
    unsigned int v;
    int digits, cur;
    if (!num || mn > mx) return 1;
    v = *num;
    if (v < mn) v = mn;
    if (v > mx) v = mx;

    digits = 1;
    { unsigned int t; for (t = mx; t >= 10; t /= 10) digits++; }
    if (digits > 9) digits = 9;
    cur = 0;
    nc_drain();

    for (;;){
        char ns[16], mk[NC_COLS + 1];
        int cx;
        nc_clear_all();
        nc_draw_title(title);
        snprintf(ns, sizeof(ns), "%0*u", digits, v);
        print_lcd(2, NC_BODY_ROW, "%s", ns);
        memset(mk, ' ', NC_COLS); mk[NC_COLS] = 0;
        cx = 2 + (digits - 1 - cur);
        if (cx >= 0 && cx < NC_COLS) mk[cx] = '^';
        print_lcd(0, NC_BODY_ROW + 1, "%s", mk);
        print_lcd(0, NC_HINT_ROW, "L/R=digit Up/Down=val Enter=OK Esc=Back");
        upd_lcd();

        {
            nc_event_t ev = nc_wait();
            if (ev == NC_EV_ENTER){
                if (v < mn) v = mn;
                if (v > mx) v = mx;
                *num = v; nc_clear_all(); upd_lcd(); return 0;
            }
            if (ev == NC_EV_ESCAPE){ nc_clear_all(); upd_lcd(); return 1; }
            if (ev == NC_EV_LEFT  && cur < digits - 1) cur++;
            if (ev == NC_EV_RIGHT && cur > 0)          cur--;

            {
                unsigned int pw = 1, dg;
                int i;
                for (i = 0; i < cur; i++) pw *= 10;
                dg = (v / pw) % 10;
                if (ev == NC_EV_UP   && dg < 9){ unsigned int nv = v + pw; v = (nv > mx) ? mx : nv; }
                if (ev == NC_EV_DOWN && dg > 0){ unsigned int nv = v - pw; v = (nv < mn) ? mn : nv; }
            }
        }
    }
}

int ncinput_uint(const char* t, unsigned int* n, unsigned int mx){
    return ncinput_uint_min(t, n, 0, mx);
}

int ncinput_int(const char* title, int* num, int mn, int mx){
    int v, range, step;
    if (!num || mn > mx) return 1;
    if (mn == mx){ *num = mn; return 0; }
    v = *num;
    if (v < mn) v = mn;
    if (v > mx) v = mx;
    range = mx - mn; step = 1;
    if (range > 1000000) step = 10000;
    else if (range > 100000) step = 1000;
    else if (range > 10000)  step = 100;
    else if (range > 1000)   step = 10;
    nc_drain();

    for (;;){
        nc_clear_all();
        nc_draw_title(title);
        print_lcd(2, NC_BODY_ROW,     "%d", v);
        print_lcd(2, NC_BODY_ROW + 1, "(step: %d)", step);
        print_lcd(0, NC_HINT_ROW, "Up/Down=step L/R=step Enter=OK Esc=Back");
        upd_lcd();

        switch (nc_wait()){
        case NC_EV_ENTER:  *num = v; nc_clear_all(); upd_lcd(); return 0;
        case NC_EV_ESCAPE: nc_clear_all(); upd_lcd(); return 1;
        case NC_EV_UP:     v = (v + step > mx) ? mx : v + step; break;
        case NC_EV_DOWN:   v = (v - step < mn) ? mn : v - step; break;
        case NC_EV_LEFT:   step = (step >= 10) ? step / 10 : 1; break;
        case NC_EV_RIGHT: {
            int ns = step * 10;
            if (ns > range) ns = range > 0 ? range : 1;
            if (ns > step) step = ns;
            break;
        }
        default: break;
        }
    }
}

/* ===================== ДЕМО-ОБРАБОТЧИКИ (ASCII для проверки) ===================== */

static void menuActivation(void){
    ncdialog_info("Activation", "Module activated.\nFirmware: %s", version);
}

static void menuInit(void){
    if (ncdialog_yesno("SPO load", "Insert SPO SD into SD2") != 0){
        ncdialog_warning("SPO load", "Cancelled by user");
        return;
    }
    ncdialog_error("SPO load", "Mount SD1 error: %d", 5);
}

static void menuRange(void){
    unsigned int first = 1, last = 100, keyCount = 500;
    if (ncinput_uint_min("Range start", &first, 1, keyCount) != 0) return;
    if (ncinput_uint_min("Range end",  &last, first, keyCount) != 0) return;
    ncdialog_info("Range", "Selected: %u ... %u", first, last);
}

static void menuAbout(void){
    ncdialog_info("About", "pd78 v%s", version);
}

static void menuMain(void){
    const char* names[] = {
        "Activation",
        "Init",
        "Range",
        "About",
        "Exit"
    };
    const menu_cb cb[] = {
        menuActivation,
        menuInit,
        menuRange,
        menuAbout,
        NULL
    };
    ncmenu("Main menu", names, cb, 5);
}

/* ===================== ЗАДАЧА ===================== */

void lcd_task_func(void *argument){
    init_lcd();
    memset((void*)LCD_FRAME_ADDRESS_SDRAM, 255, LCD_BUFFER_SIZE);

    /* --- ОТЛАДКА: проверяем, что print_lcd/upd_lcd вообще работают --- */
    print_lcd(0, 0, "BOOT OK");
    upd_lcd();
    osDelay(1500);

    ncmenu_init();

    while (1){
        menuMain();          /* блокирующий вызов: рисует меню и ждёт кнопок */
        ncmenu_deinit();
        memset((void*)LCD_FRAME_ADDRESS_SDRAM, 255, LCD_BUFFER_SIZE);
        osDelay(100);
    }
}