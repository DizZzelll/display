#include "main.h"
#include <stdbool.h>

#ifndef SDRAM_H
#define SDRAM_H

#include "user.h"

#define SDRAM_BASE_ADDR   0xC0000000
#define LCD_FRAME_ADDRESS_SDRAM      SDRAM_BASE_ADDR
#define TEST_SIZE         0x10  // 4KB для теста

extern SDRAM_HandleTypeDef hsdram1;
#define SdramHandle        hsdram1

/**
  * @brief  FMC SDRAM Mode definition register defines
  */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

/**
 * @brief 
 * Set the refresh rate counter 
 * (15.62 us x Freq) - 20 
 * */

/* RefreshCount для 4096 строк */
//hsdram1.Init.RefreshCount = 1172;  // (75e6 × 0.064) / 4096
#define REFRESH_COUNT           ((uint32_t)1172)   /* SDRAM refresh counter */
#define SDRAM_TIMEOUT           ((uint32_t)0xFFFF)


#ifdef __cplusplus
extern "C" {
#endif

bool test_sdram_basic(void);
void user_SDRAM_Initialization_sequence(uint32_t RefreshCount);

#ifdef __cplusplus
}
#endif

#endif