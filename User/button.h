#ifndef BUTTON_H
#define BUTTON_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

typedef struct {
    uint16_t up       : 1;
    uint16_t down     : 1;
    uint16_t left     : 1;
    uint16_t right    : 1;
    uint16_t enter    : 1;
    uint16_t escape   : 1;
    
    // Резервные биты
    uint16_t reserved : 10;
} ButtonsBits;

// Union для удобной работы
typedef union {
    uint16_t bvalue;        // Доступ ко всему 16-битному значению
    ButtonsBits bbits;      // Доступ к отдельным битам
} Buttons;

#ifdef __cplusplus
}
#endif

#endif