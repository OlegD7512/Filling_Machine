#ifndef __THREADS_H
#define __THREADS_H

#include "main.h"
#include "../Inc/Interactions.h"

#define PUMPING_STATUS  HAL_GPIO_ReadPin(DI4_GPIO_Port,DI5_Pin) 

void ModBusThreadHandler(void *argument);
void MainThreadHandler(void *argument);

// Прототипы вспомогательных функций (для задачи: MainThreadHandler)
void RTC_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void RTC_SetDate(uint8_t day, uint8_t month, uint8_t year);
void updIfaceElem(IfaceElementID id, float val); 
static void pollFrequencyConverters(CurrState* stats);
static void checkDigitalInputsStatus(uint16_t* StatusDidgImput);
static void pollConvertersStatus(CurrState* stats);
static void pollSingleSensor(uint8_t address, uint16_t registerNum, float* tempValue, IfaceElementID displayElem, float* minTemp);
static void pollTemperatureSensors(CurrState* stats);
static void processSettingsUpdates(void);

// Прототипы вспомогательных функций (для задачи: ModBusThreadHandler)
static void LoadInitialSettings(void);
static void ProcessSettingsUpdate(void);
static void UpdateWorkTimeStats(void);
static void ProcessControlCommands(void);


#endif