#ifndef __MB_FUNC_H
#define __MB_FUNC_H

#include "main.h"


#define NO_ERR 0xF5					// ошибок НЕТ (Modebus) 

#define maxDelayResponse 2000   // максимальное время задрежки ответа устройчтва 

void MbInit(void);
int8_t MbReadFloat (uint8_t slaveAddr, uint16_t regAddr, float* val);
int8_t MbWriteFloat (uint8_t slaveAddr, uint16_t regAddr, float val);
uint8_t MbReadInt16 (uint8_t slaveAddr, uint16_t regAddr, uint16_t* val);
int8_t MbWriteInt16 (uint8_t slaveAddr, uint16_t regAddr, uint16_t val);

#endif /* __MB_FUNC_H */

