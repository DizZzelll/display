#ifndef ETH_H
#define ETH_H
/*
при конфигурации ETH c FreeRTOS создается три потока:
"EthIf" - Интерфейсный поток Ethernet
Обрабатывает передачу и прием Ethernet-кадров
Связывает драйвер Ethernet HAL с LwIP

"EthLink" - Поток отслеживания состояния линии
Мониторит состояние физического соединения (link status)
Обрабатывает события подключения/отключения кабеля

"tcpip_thread" - Главный поток LwIP TCP/IP
Создается автоматически LwIP при вызове tcpip_init()
Обрабатывает все TCP/IP протоколы (IP, TCP, UDP, ICMP, IGMP)
Наиболее важный системный поток LwIP
*/
#ifdef __cplusplus
extern "C" {
#endif
void phy_init(void);
#ifdef __cplusplus
}
#endif

#endif