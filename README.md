#### Display_pd78

Весь созданный код (пользовательский) содержится в папке User, все остальное это сгенерированный код CubeMx. Интеграция кода пользовательского осущесвлялась вызовами функций в коде сгененрированным CubeMx, старался это делать в местаx /**USER CODE BEGIN**/  /***USER CODE END***/ где это было возможно.

Есть git, там есть подробное описание что делалось

В проекте 5 потоков FreeRTOS пользовательских , 3 системных потока Ehternet,  все они живут пока включено устойство, работают в режиме вытеснения ,генерируются с помощью CubeMx

Пользовательские потоки:

  /* creation of defaultTask */

  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes); // поток для инициализации LWIP

  /* creation of lcd_task */

  lcd_taskHandle = osThreadNew(lcd_task_func, NULL, &lcd_task_attributes); // поток для работы с LCD

  /* creation of HttpTask */

  HttpTaskHandle = osThreadNew(HttpTask_func, NULL, &HttpTask_attributes); // для инициализации HTTP сервера

  /* creation of button_task */

  button_taskHandle = osThreadNew(button_task_func, NULL, &button_task_attributes); // поток для работы с кнопками

  /* creation of touch_task */

  touch_taskHandle = osThreadNew(touch_task_func, NULL, &touch_task_attributes);  // поток для обработки сенсора

Системные потоки Ethernet:

"EthIf" - Интерфейсный поток Ethernet Обрабатывает передачу и прием Ethernet-кадров Связывает драйвер Ethernet HAL с LwIP

"EthLink" - Поток отслеживания состояния линии Мониторит состояние физического соединения (link status) Обрабатывает события подключения/отключения кабеля

"tcpip_thread" - Главный поток LwIP TCP/IP Создается автоматически LwIP при вызове tcpip_init() Обрабатывает все TCP/IP протоколы (IP, TCP, UDP, ICMP, IGMP) Наиболее важный системный поток LwIP
