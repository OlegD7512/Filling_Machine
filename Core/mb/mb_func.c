/**
  ******************************************************************************
  * @file    mb_func.c
  * @brief   Содержит функции для чтения и записи регистров 
	*	modbus устройств 
  ******************************************************************************
  ******************************************************************************
  */
#include "mb_func.h"

#include "Modbus.h"

#include "stdbool.h"

extern UART_HandleTypeDef huart3;

modbusHandler_t ModbusH;

static uint16_t mbMainBuf	[64];
static uint16_t mbInBuf		[32];
static uint16_t mbOutBuf	[32];
static modbus_t sendTel;
static modbus_t recvTel;

/**
* @brief Инициализация modbus
* @param нет
* @retval нет
*/
void MbInit(void){
	ModbusH.uModbusType = MB_MASTER;
  ModbusH.port =  &huart3;
  ModbusH.u8id = 0; // For master it must be 0
  ModbusH.u16timeOut = 1000;
  ModbusH.EN_Port = RS485CTRL_GPIO_Port;
  ModbusH.EN_Pin = RS485CTRL_Pin;
  ModbusH.u16regs = mbMainBuf;
  ModbusH.u16regsize= sizeof(mbMainBuf)/sizeof(mbMainBuf[0]);
  ModbusH.xTypeHW = USART_HW;
  //Initialize Modbus library
  ModbusInit(&ModbusH);
  //Start capturing traffic on serial Port
  ModbusStart(&ModbusH);
}

/**
* @brief Чтение float числа из двух регистров
* @param slaveAddr: адрес целевого устройства
* @param regAddr: адрес первого из пары регистров
* @param val: указатель на перменную, в которую записываем ответ целевого устройства
* @retval статус ошибки, либо отсутствия ошибки (NO_ERR)
*/
int8_t MbReadFloat (uint8_t slaveAddr, uint16_t regAddr, float* val){
	sendTel.u8id = slaveAddr; 						// адрес устройства
  sendTel.u8fct = MB_FC_READ_REGISTERS; // чтение holding register
  sendTel.u16RegAdd = regAddr;					// адрес начального регистра
  sendTel.u16CoilsNo = 2; 							// сколько регистров считывать
  sendTel.u16reg = mbInBuf; 						// указатель на массив, куда класть данные
	
	ModbusQuery(&ModbusH, sendTel);
	int8_t err = ulTaskNotifyTake(pdTRUE, maxDelayResponse);
	if(err != ERR_OK_QUERY){
		return err;	 //выйдем если нет ответа значение будет - 0
	}
	else{
		uint16_t* pVal= (uint16_t*) val;		// без этого приведения к типу uint16_t и копирования адреса не получается и 
	
	*pVal 		= mbInBuf[0];  // ну и в этот адрес производим запись
	*(pVal+1) = mbInBuf[1];
	
	return err;
	}
}


/**
* @brief Запись float числа в два регистра
* @param slaveAddr: адрес целевого устройства
* @param regAddr: адрес первого из пары регистров
* @param val: число для записи
* @retval статус ошибки, либо отсутствия ошибки (NO_ERR)
*/
int8_t MbWriteFloat (uint8_t slaveAddr, uint16_t regAddr, float val){
	sendTel.u8id = slaveAddr; 											// адрес устройства
  sendTel.u8fct = MB_FC_WRITE_MULTIPLE_REGISTERS; // запись holding register
  sendTel.u16RegAdd = regAddr;										// адрес начального регистра
  sendTel.u16CoilsNo = 2; 												// сколько регистров считывать
  sendTel.u16reg = mbOutBuf; 											// указатель на массив, куда класть данные
	
	uint16_t* pVal = (uint16_t*) &val;
	
	mbOutBuf[0] = *pVal;
	mbOutBuf[1] = *(pVal+1);
	
	ModbusQuery(&ModbusH, sendTel);
	int8_t err = ulTaskNotifyTake(pdTRUE, maxDelayResponse);
	if(err != ERR_OK_QUERY) return err;
	else return err;
}


/**
* @brief Чтение uint16_t числа из двух регистров
* @param slaveAddr: адрес целевого устройства
* @param regAddr: адрес первого из пары регистров
* @param val: указатель на перменную типа float	, в которую записываем ответ целевого устройства
* @retval статус ошибки, либо отсутствия ошибки (NO_ERR)
*/
uint8_t MbReadInt16 (uint8_t slaveAddr, uint16_t regAddr, uint16_t* val){
	sendTel.u8id = slaveAddr; 						// адрес устройства
  sendTel.u8fct = MB_FC_READ_REGISTERS; // чтение holding register
  sendTel.u16RegAdd = regAddr;					// адрес начального регистра
  sendTel.u16CoilsNo = 1; 							// сколько регистров считывать
  sendTel.u16reg = mbInBuf; 						// указатель на массив, куда класть данные
	
	ModbusQuery(&ModbusH, sendTel);
	int8_t err = ulTaskNotifyTake(pdTRUE, maxDelayResponse);
	
	*val = mbInBuf[0];  // ну и в этот адрес производим запись
	
	//if(err != ERR_OK_QUERY) return err;
	return err;
}

/**
* @brief Запись float числа в два регистра
* @param slaveAddr: адрес целевого устройства
* @param regAddr: адрес первого из пары регистров
* @param val: число для записи
* @retval статус ошибки, либо отсутствия ошибки (NO_ERR)
*/

int8_t MbWriteInt16 (uint8_t slaveAddr, uint16_t regAddr, uint16_t val){
	sendTel.u8id = slaveAddr; 											// адрес устройства
  sendTel.u8fct = MB_FC_WRITE_MULTIPLE_REGISTERS; // запись holding register
  sendTel.u16RegAdd = regAddr;										// адрес начального регистра
  sendTel.u16CoilsNo = 1; 												// сколько регистров считывать
  sendTel.u16reg = mbOutBuf; 											// указатель на массив, куда класть данные
	
	mbInBuf[0] = val;
	
	ModbusQuery(&ModbusH, sendTel);
	int8_t err = ulTaskNotifyTake(pdTRUE, maxDelayResponse);
	//if(err != ERR_OK_QUERY)	return err;
	return err;
}



