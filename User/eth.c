#include "eth.h"
#include "main.h"

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "queue.h"
#include "semphr.h"
/**
 * @brief инициализация ksz8041 (страница даташита 53)
 * 
 */
void phy_init(void){
      /*
      Плата ПД78
      KSZ8041 1310 nm set Default:
      RMII (default) -> EthPHY_ADress = 00110
      DUPLEX mode = FULL_DUPLEX
      ISOLATE mode = Enabled
      SPEED mode = 100 Mbps
      Disable Auto-Negotiation
      RESET - >10 ms on PowerON
      */

    HAL_GPIO_WritePin(eth_rst_GPIO_Port, eth_rst_Pin,  GPIO_PIN_RESET);
    osDelay(15);
    HAL_GPIO_WritePin(eth_rst_GPIO_Port, eth_rst_Pin,  GPIO_PIN_SET);
    osDelay(1);
}