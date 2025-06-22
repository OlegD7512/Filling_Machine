

#include "threads.h"
#include "cmsis_os.h"
#include "task.h"
#include "../Inc/Interactions.h"
#include "queue.h"
#include "Modbus.h"
#include "string.h"
#include "../mb/mb_func.h"
#include "stdio.h"
#include "eeprom.h"



//#include "eeprom.h"

QueueHandle_t sendCmdQ;
QueueHandle_t setSettsQ;
QueueHandle_t viewStatQ;
QueueHandle_t onSettsChngQ;


BaseSetts currSetts;
CurrState currStats;		// структура со всеми параметрами устройств

extern TIM_HandleTypeDef htim6;
extern RTC_HandleTypeDef hrtc;


//uint16_t ModbusBuf[16];
//uint16_t ModbusDATA[16];
//uint16_t ModbusDATAin[16];
//modbus_t telegram[2];
//uint32_t u32NotificationValue;

//uint8_t  ModebusErrCode;
//float lastWorkTime=0;



/**
 * @brief Таблица виртуальных адресов для работы с EEPROM (Flash эмуляцией)
 * 
 * @details Содержит перечень всех переменных, сохраняемых в энергонезависимую память.
 * Каждой переменной соответствует уникальный виртуальный адрес.
 * 
 * @note Порядок адресов должен соответствовать порядку обращения к переменным в программе.
 *       Размер массива должен строго соответствовать NB_VAR.
 * 
 * @value EE_ADD_SPEED_1   Адрес для хранения скорости 1
 * @value EE_ADD_SPEED_2   Адрес для хранения скорости 2
 * @value EE_ADD_TARGET_T  Адрес для хранения целевой температуры
 * @value EE_ADD_DELTA_T   Адрес для хранения дельты температуры
 */
uint16_t VirtAddVarTab[NB_VAR] = {
    EE_ADD_SPEED_1,     ///< Адрес скорости 1
    EE_ADD_SPEED_2,     ///< Адрес скорости 2
    EE_ADD_TARGET_T,    ///< Адрес целевой температуры
    EE_ADD_DELTA_T      ///< Адрес дельты температуры
};



//----------------------------------------------------------------------
// MODULE: ОЧЕРЕДИ (объявление)
//----------------------------------------------------------------------

/**
 * @brief Создает набор очередей для межзадачного взаимодействия
 * 
 * @details Функция инициализирует все очереди, используемые в системе:
 *          - Очередь для отправки команд
 *          - Очередь для установки настроек
 *          - Очередь для обновления статуса интерфейса
 *          - Очередь для обработки изменений настроек
 * 
 * @note В случае ошибки создания любой из очередей система продолжит работу,
 *       но соответствующий функционал будет недоступен. В реальной системе
 *       следует добавить обработку таких ошибок (например, аварийную остановку).
 * 
 * @warning Размеры очередей подобраны эмпирически и могут требовать корректировки
 *          в зависимости от нагрузки системы.
 * 
 * @param None
 * @retval None
 */
void CreateQueues(void) {
    /* Очередь для отправки команд (размер: 5 элементов типа uint8_t) */
    sendCmdQ = xQueueCreate(5, sizeof(uint8_t));
    if (sendCmdQ == NULL) {
        // Обработка ошибки создания очереди
        //printf("Failed to create sendCmdQ\n");
    }
    
    /* Очередь для установки базовых настроек (размер: 3 элемента типа BaseSetts) */
    setSettsQ = xQueueCreate(3, sizeof(BaseSetts));
    if (setSettsQ == NULL) {
        // Обработка ошибки создания очереди
        //printf("Failed to create setSettsQ\n");
    }
    
    /* Очередь для обновления статуса интерфейса (размер: 10 элементов типа IfaceUpd) */
    viewStatQ = xQueueCreate(10, sizeof(IfaceUpd));
    if (viewStatQ == NULL) {
        // Обработка ошибки создания очереди
        //printf("Failed to create viewStatQ\n");
    }
    
    /* Очередь для обработки изменений настроек (размер: 3 элемента типа BaseSetts) */
    onSettsChngQ = xQueueCreate(3, sizeof(BaseSetts));
    if (onSettsChngQ == NULL) {
        // Обработка ошибки создания очереди
        //printf("Failed to create onSettsChngQ\n");
    }
}



//----------------------------------------------------------------------
// MODULE: ОСНОВНАЯ ЗАДАЧА (основной поток)
//----------------------------------------------------------------------

void MainThreadHandler(void *argument)
{
	
    /* Инициализация временных меток */
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = 10 / portTICK_PERIOD_MS;

    /* Загрузка начальной конфигурации */
    LoadInitialSettings();

    /* Основной цикл обработки */
    for(;;) {
        ProcessSettingsUpdate();
        UpdateWorkTimeStats();
        ProcessControlCommands();
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
//----------------------------------------------------------------------
// MODULE: ЗАДАЧА ВЗАИМОДЕЙСТВИЯ ПО MODEBUS
//----------------------------------------------------------------------


/**
 * @brief Задача взаимодействия по протоколу Modbus
 * 
 * @details Периодически опрашивает частотные преобразователи и температурные датчики,
 *          обновляет текущее состояние системы и отправляет данные на отображение.
 *          Также обрабатывает команды изменения настроек из очереди.
 * 
 * @param argument Неиспользуемый параметр (требование FreeRTOS API)
 * 
 * @note Частота опроса: 1 секунда (1000 мс)
 * @note Приоритет задачи должен быть ниже, чем у задач с критичным временем отклика
 * @warning При ошибках Modbus счетчики ошибок не инкрементируются (закомментировано)
 */
void ModBusThreadHandler(void *argument)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000); // Период 1000 мс
    xLastWakeTime = xTaskGetTickCount();
    uint16_t readVar;
		uint16_t StatusDidgImput;
    
    for(;;)
    {
        /*---- Опрос выходной частоты частотных преобразователей ----*/
        pollFrequencyConverters(&currStats);
        
        /*---- Проверка состояния дискретных входов ПЧ ----*/
        checkDigitalInputsStatus(&StatusDidgImput);
        
        /*---- Опрос параметров датчиков и ПЧ (если входы в базовом состоянии) ----*/
        if(StatusDidgImput == dfltStatusDidgIn)
        {
            pollConvertersStatus(&currStats);
            pollTemperatureSensors(&currStats);
            
            vTaskDelayUntil(&xLastWakeTime, xFrequency); // Точная задержка
        }
        
        /*---- Обработка новых настроек из очереди ----*/
        processSettingsUpdates();
    }
}



//----------------------------------------------------------------------
// MODULE: ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ (прототипы в файле threads.h)
//----------------------------------------------------------------------


//------- для потока 

/**
 * @brief Устанавливает время в модуле RTC.
 * 
 * @details Функция инициализирует структуру RTC_TimeTypeDef переданными параметрами времени
 * и вызывает HAL-функцию для установки времени в RTC. В случае ошибки вызывает обработчик ошибок.
 * 
 * @note По умолчанию используются следующие настройки:
 *       - 12-часовой формат времени (AM)
 *       - Без перехода на летнее время
 *       - Сброс операции хранения
 * 
 * @param hours Часы (0-12 в 12-часовом формате или 0-23 в 24-часовом формате)
 * @param minutes Минуты (0-59)
 * @param seconds Секунды (0-59)
 * 
 * @retval None
 * 
 * @warning Если HAL_RTC_SetTime вернет ошибку, будет вызван Error_Handler()
 * @warning Для 12-часового формата необходимо изменить sTime.TimeFormat на RTC_HOURFORMAT12_AM 
 */
void RTC_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    RTC_TimeTypeDef sTime = {0};
    
    sTime.Hours = hours;
    sTime.Minutes = minutes;
    sTime.Seconds = seconds;
    sTime.TimeFormat = RTC_HOURFORMAT_24;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief Устанавливает дату в модуле RTC.
 * 
 * @details Функция заполняет структуру RTC_DateTypeDef переданными параметрами даты
 * и вызывает HAL-функцию для установки даты в RTC. В случае ошибки вызывает обработчик ошибок.
 * 
 * @note Для STM32 год указывается в диапазоне 0-99 (соответствует 2000-2099 годам).
 *       День недели (WeekDay) устанавливается как понедельник (RTC_WEEKDAY_MONDAY),
 *       так как это поле либо игнорируется, либо может быть вычислено автоматически.
 * 
 * @param day День месяца (1-31)
 * @param month Месяц (1-12)
 * @param year Год (0-99, соответствует 2000-2099)
 * 
 * @retval None
 * 
 * @warning Если HAL_RTC_SetDate вернет ошибку, будет вызван Error_Handler()
 */
void RTC_SetDate(uint8_t day, uint8_t month, uint8_t year)
{
    RTC_DateTypeDef sDate = {0};
    
    sDate.WeekDay = RTC_WEEKDAY_MONDAY; // Можно вычислять или игнорировать
    sDate.Month = month;
    sDate.Date = day;
    sDate.Year = year; // Для STM32 год 0-99 (2000-2099)
    
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }
}



/**
 * @brief Функция обновления данных интерфейса на LCD
 * 
 * @param id Идентификатор элемента интерфейса из перечисления IfaceElementID
 * @param val Новое значение для элемента интерфейса
 * 
 * @details Функция выполняет следующие действия:
 * 1. Создает структуру IfaceUpd с полученными параметрами
 * 2. Отправляет эту структуру в очередь viewStatQ для обработки в потоке интерфейса
 * 
 * @note Таймаут отправки в очередь установлен в 100 мс (pdMS_TO_TICKS(100))
 * 
 * @warning При переполнении очереди или превышении таймаута данные могут быть потеряны
 * 
 * @see IfaceElementID, IfaceUpd, viewStatQ
 */
void updIfaceElem(IfaceElementID id, float val)
{
    IfaceUpd iUpd;          ///< Временная структура для передачи данных
    iUpd.elemId = id;       ///< Установка идентификатора элемента
    iUpd.val = val;         ///< Установка нового значения
    
    // Отправка данных в очередь отображения
    xQueueSend(viewStatQ, &iUpd, pdMS_TO_TICKS(100));
}


/**
 * @brief Опрашивает выходную частоту всех частотных преобразователей
 * @param stats Указатель на структуру для хранения текущего состояния
 */
static void pollFrequencyConverters(CurrState* stats)
{
    uint16_t readVar;
		uint8_t  ModebusErrCode;
    
    // ПЧ 1
    ModebusErrCode = MbReadInt16(AddrFrqCnvr_1, regOutFrqFC, &readVar);
    stats->SpeedCF_1 = (float)readVar;
    if(ModebusErrCode == NO_ERR) updIfaceElem(SPEED1, stats->SpeedCF_1 / 100);
    
    // ПЧ 2
    ModebusErrCode = MbReadInt16(AddrFrqCnvr_2, regOutFrqFC, &readVar);
    stats->SpeedCF_2 = (float)readVar;
    if(ModebusErrCode == NO_ERR) updIfaceElem(SPEED2, stats->SpeedCF_2 / 100);
    
    // ПЧ 3
    ModebusErrCode = MbReadInt16(AddrFrqCnvr_3, regOutFrqFC, &readVar);
    stats->SpeedCF_3 = (float)readVar;
    if(ModebusErrCode == NO_ERR) updIfaceElem(SPEED3, stats->SpeedCF_3 / 100);
}

/**
 * @brief Проверяет состояние дискретных входов частотных преобразователей
 * @details Если на первом ПЧ вход в базовом состоянии, проверяет остальные
 */
static void checkDigitalInputsStatus(uint16_t* StatusDidgImput)
{
	  
    MbReadInt16(AddrFrqCnvr_1, regStatDiFC, StatusDidgImput);
    if(*StatusDidgImput == dfltStatusDidgIn)
        MbReadInt16(AddrFrqCnvr_2, regStatDiFC, StatusDidgImput);
}

/**
 * @brief Опрашивает текущее состояние частотных преобразователей
 * @param stats Указатель на структуру для хранения текущего состояния
 */
static void pollConvertersStatus(CurrState* stats)
{
    uint16_t readVar;
    uint8_t  ModebusErrCode;
	
    ModebusErrCode = MbReadInt16(AddrFrqCnvr_1, regCurrStatFC, &readVar);
    stats->statusFC_1 = readVar;
    if(ModebusErrCode == NO_ERR) updIfaceElem(STAT_FC1, stats->statusFC_1);
    
    ModebusErrCode = MbReadInt16(AddrFrqCnvr_2, regCurrStatFC, &readVar);
    stats->statusFC_2 = readVar;
    if(ModebusErrCode == NO_ERR) updIfaceElem(STAT_FC2, stats->statusFC_2);
    
    ModebusErrCode = MbReadInt16(AddrFrqCnvr_3, regCurrStatFC, &readVar);
    stats->statusFC_3 = readVar;
    if(ModebusErrCode == NO_ERR) updIfaceElem(STAT_FC3, stats->statusFC_3);
}

/**
 * @brief Опрашивает один температурный датчик и обновляет минимальную температуру
 * @param address Адрес устройства
 * @param registerNum Номер регистра
 * @param tempValue Указатель для сохранения температуры
 * @param displayElem Элемент интерфейса для обновления
 * @param minTemp Указатель на переменную с минимальной температурой
 */
static void pollSingleSensor(uint8_t address, uint16_t registerNum, 
                           float* tempValue, IfaceElementID displayElem,
                           float* minTemp)
{
		uint8_t  ModebusErrCode;
    ModebusErrCode = MbReadFloat(address, registerNum, tempValue);
    if(ModebusErrCode == NO_ERR)
    {
        updIfaceElem(displayElem, *tempValue);
        if(*tempValue < *minTemp) *minTemp = *tempValue;
    }
}

/**
 * @brief Опрашивает все температурные датчики и определяет минимальную температуру
 * @param stats Указатель на структуру для хранения текущего состояния
 */
static void pollTemperatureSensors(CurrState* stats)
{
    // Опрос всех датчиков температуры
    pollSingleSensor(AddrTRM_1, regTRM_sensor1, &stats->temperPump_1, TEMP1, &stats->tempCurr);
    pollSingleSensor(AddrTRM_1, regTRM_sensor2, &stats->temperHoseToBarrel_1, TEMP4, &stats->tempCurr);
    pollSingleSensor(AddrTRM_2, regTRM_sensor1, &stats->temperHoseToReactor_1, TEMP7, &stats->tempCurr);
    updIfaceElem(TEMP8, stats->temperHoseToReactor_1); // Дублируем для TEMP8
    pollSingleSensor(AddrTRM_2, regTRM_sensor2, &stats->temperPump_2, TEMP2, &stats->tempCurr);
    pollSingleSensor(AddrTRM_3, regTRM_sensor1, &stats->temperHoseToBarrel_2, TEMP5, &stats->tempCurr);
    pollSingleSensor(AddrTRM_3, regTRM_sensor2, &stats->temperReactorTop, TEMP3, &stats->tempCurr);
    pollSingleSensor(AddrTRM_4, regTRM_sensor1, &stats->temperReactorMid, TEMP6, &stats->tempCurr);
    pollSingleSensor(AddrTRM_4, regTRM_sensor2, &stats->temperReactorLow, TEMP9, &stats->tempCurr);
    
    // Отправка минимальной температуры
    updIfaceElem(TEMP_CURR, stats->tempCurr);
}



/**
 * @brief Обрабатывает новые настройки из очереди
 * @details Записывает новые уставки температуры во все ТРМ
 */
static void processSettingsUpdates(void)
{
    if(xQueueReceive(onSettsChngQ, &currSetts, 0) == pdTRUE)
    {
        // Запись уставки во все ТРМ
        for(uint8_t i = AddrTRM_1; i <= AddrTRM_4; i++)
        {
            MbWriteFloat(i, regTRM_SP1, currSetts.targetTemp);
            MbWriteFloat(i, regTRM_SP2, currSetts.targetTemp);
        }
    }
}


//-----для потока 


/**
 * @brief Загружает начальные настройки из EEPROM
 * @details Считывает целевую температуру из энергонезависимой памяти и обновляет интерфейс
 * @retval None
 * @note Вызывается однократно при старте системы
 */
static void LoadInitialSettings(void)
{
    uint16_t data;
    if(EE_ReadVariable(EE_ADD_TARGET_T, &data) != HAL_OK) {
        Error_Handler();
    }
    currSetts.targetTemp = (float)data;
    updIfaceElem(TEMP_TARGET, currSetts.targetTemp);
}

/**
 * @brief Обрабатывает обновления настроек из очереди
 * @details При получении новых настроек:
 * - Сохраняет в EEPROM
 * - Обновляет интерфейс
 * - Отправляет в очередь для Modbus-устройств
 * @retval None
 */
static void ProcessSettingsUpdate(void)
{
    if(xQueueReceive(setSettsQ, &currSetts, 0) == pdTRUE) {
        if(EE_WriteVariable(EE_ADD_TARGET_T, (uint16_t)currSetts.targetTemp) != HAL_OK) {
            Error_Handler();
        }
        updIfaceElem(TEMP_TARGET, currSetts.targetTemp);
        xQueueSend(onSettsChngQ, &currSetts, pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Обновляет статистику времени работы
 * @details Вычисляет и отображает время в рабочем режиме:
 * - Фиксирует время старта
 * - Обновляет отображение раз в секунду
 * - Сбрасывает при выходе из режима
 * @retval None
 */
static void UpdateWorkTimeStats(void)
{
		static TickType_t startWorkTime = 0;  ///< Время начала рабочего режима  ?????
	  static TickType_t lastUpdateTime = 0;
    const TickType_t tickTime_1s = 1000 / portTICK_PERIOD_MS;

    if (currStats.statusFC_1 == WORK) {
        if (startWorkTime == 0) {
            startWorkTime = xTaskGetTickCount();
        }
        
        TickType_t currentTime = xTaskGetTickCount();
        currStats.currWorkTime = (float)(currentTime - startWorkTime);
        
        if ((currentTime - lastUpdateTime) >= tickTime_1s) {
            updIfaceElem(TIME_WORK, currStats.currWorkTime/1000);
            lastUpdateTime = currentTime;
        }
    } 
    else if (startWorkTime != 0) {
        currStats.currWorkTime = 0;
        startWorkTime = 0;
    }
}

/**
 * @brief Обрабатывает команды управления из очереди
 * @details В текущей реализации обрабатывает:
 * - RESET_VOL_CMD: Сброс накопленного объема
 * @retval None
 */
static void ProcessControlCommands(void)
{
    uint8_t cmd;
    if(xQueueReceive(sendCmdQ, &cmd, 0) == pdTRUE) {
        if(cmd == RESET_VOL_CMD) {
            /* Реализация сброса объема */
        }
    }
}
