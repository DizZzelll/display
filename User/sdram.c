#include "sdram.h"


/**
 * @brief чтение запись bank1 sdram адрес 0xC0000000
 * 
 * @return true - чтение запись прошла успешно
 * @return false - чтение запись прошла с ошибками
 */
bool test_sdram_basic(void) {
    volatile uint32_t* sdram = (uint32_t*)SDRAM_BASE_ADDR;
    
    printf("Testing SDRAM at 0x%08lX...\n", (uint32_t)sdram);
    
    sdram[0] = 1;
    // Тест записи
    printf("Writing test pattern...\n");
    for(uint32_t i = 0; i < TEST_SIZE / 4; i++) {
        sdram[i] = 0x12345678 + i;  // Простой паттерн
    }
    
    // Тест чтения
    printf("Verifying data...\n");
    for(uint32_t i = 0; i < TEST_SIZE / 4; i++) {
        if(sdram[i] != (0x12345678 + i)) {
            printf("SDRAM ERROR at 0x%08lX: wrote 0x%08lX, read 0x%08lX\n",
                   (uint32_t)&sdram[i], (0x12345678 + i), sdram[i]);
            HARDFAULT;
            return false;
        }
    }
    
    printf("SDRAM basic test PASSED!\n");
    return true;
}

/**
  * @brief  Programs the SDRAM device.
  * @param  RefreshCount: SDRAM refresh counter value
  */
void user_SDRAM_Initialization_sequence(uint32_t RefreshCount)
{
  __IO uint32_t tmpmrd = 0;
  FMC_SDRAM_CommandTypeDef Command = {0};

  /* Step 1:  Configure a clock configuration enable command */
  Command.CommandMode             = FMC_SDRAM_CMD_CLK_ENABLE;
  Command.CommandTarget           = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber       = 1;
  Command.ModeRegisterDefinition  = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&SdramHandle, &Command, SDRAM_TIMEOUT);

  /* Step 2: Insert 100 us minimum delay */
  /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
  HAL_Delay(1);

  /* Step 3: Configure a PALL (precharge all) command */
  Command.CommandMode             = FMC_SDRAM_CMD_PALL;
  Command.CommandTarget           = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber       = 1;
  Command.ModeRegisterDefinition  = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&SdramHandle, &Command, SDRAM_TIMEOUT);

  /* Step 4: Configure an Auto Refresh command */
  Command.CommandMode             = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  Command.CommandTarget           = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber       = 8;
  Command.ModeRegisterDefinition  = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&SdramHandle, &Command, SDRAM_TIMEOUT);

  /* Step 5: Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |
           SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
           SDRAM_MODEREG_CAS_LATENCY_3           |
           SDRAM_MODEREG_OPERATING_MODE_STANDARD |
           SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

  Command.CommandMode             = FMC_SDRAM_CMD_LOAD_MODE;
  Command.CommandTarget           = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber       = 1;
  Command.ModeRegisterDefinition  = tmpmrd;

  /* Send the command */
  HAL_SDRAM_SendCommand(&SdramHandle, &Command, SDRAM_TIMEOUT);

  /* Step 6: Set the refresh rate counter */
  /* Set the device refresh rate */
  HAL_SDRAM_ProgramRefreshRate(&SdramHandle, RefreshCount);
}