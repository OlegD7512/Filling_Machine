#ifndef __INTERACTIONS_H
#define __INTERACTIONS_H

#include "main.h"


#define AddrTRM_1 11				//	(11) 
#define AddrTRM_2 12				//	(12)	
#define AddrTRM_3 13				//	(13)
#define AddrTRM_4 14				//	(13)

#define regTRM_sensor1 0x100D				// Одинаковы 
#define regTRM_sensor2 0x100F				//					для 
#define regTRM_SP1 0x1011						//								каждого
#define regTRM_SP2 0x1013						//													2ТРМ1


#define AddrFrqCnvr_1 21		//	(21)
#define AddrFrqCnvr_2 22		//	(21) должно быть 22
#define AddrFrqCnvr_3 23		//	(21) должно быть 23

#define regOutFrqFC 0x703B					// выходная частота (как ни странно одинаков для скалярного и векторного ПЧ)
//#define regRecFrqFC 0x2001					// регистр для записи заначения выходной частоты 
#define regStatDiFC 0x0015					// состояние дискретных входов (0x7008 для веторного)
#define regCurrStatFC 0x001C			 	//	текущее состояние ПЧ
#define regCurrTimeFC 0x7019
//#define regRttSpdFC 0xF105				// скороть вращения двигателя 
#define dfltStatusDidgIn 0x0004			// ответ ПЧ по состояню дискрет. выходов в трехпроводном режиме




#define 	EE_ADD_SPEED_1				0x00				 
#define 	EE_ADD_SPEED_2 				0x01				 
#define 	EE_ADD_TARGET_T	    	0x02
#define 	EE_ADD_DELTA_T			 	0x03
#define 	NB_VAR								0x04


typedef enum {
	STOP  			  =	0x00U,
	WORK          =	0x01U,
	HEATING       =	0x02U,
	READY 				=	0x03U,
	PUMPING				=	0x04U,
	HW_ERR				=	0x05U,
	LOW_TEMP_ERR =	0x06U
}MachineState;

typedef enum {
	DSBL,					// выключен
	ENBL					// включен
}StatusVar;		// перечисление выкл/вел

typedef enum {
	SPEED1,
	SPEED2,
	SPEED3,
	STAT_FC1,
	STAT_FC2,
	STAT_FC3,
	TEMP1,
	TEMP2,
	TEMP3,
	TEMP4,
	TEMP5,
	TEMP6,
	TEMP7,
	TEMP8,
	TEMP9,
	TEMP_CURR,
	CURR_VOL_A,
	CURR_VOL_B,
	CURR_VOL_S,
	VOL_A,
	VOL_B,
	SPEED,
	TEMP_TARGET,		// это уставка из ТРМов (точнее уставка реактора средней части)
	//SET_TARGET_T,		// это уставка из настоек (для удобства, что бы видеть что было раньше)
	TEMP_A,
	CURR_STATE,
	TIME_WORK,				// время работы насосов
	TIME_RTC					// время и дата
}IfaceElementID;	// InterfaceElementID


typedef struct IfaceUpd{
	IfaceElementID 	elemId;
	float 					val;			//температура шланга (например)
}IfaceUpd;  // InterfaceUpdate

typedef enum uint8_t{
	STOP_CMD 			= 0x00,
	START_CMD			= 0x01,
	RESET_VOL_CMD	= 0x02,
	NOP_CMD = 0xFF
}UserCmd;

typedef struct CurrState{
	float 				temperPump_1;  		        //температура насоса - 1
	float 				temperHoseToBarrel_1;			//температура всасывающего шланга - 1
	float 				temperHoseToReactor_1;			//температура напорного шланга - 1
	float 				temperPump_2;  		        //температура насоса - 2
	float 				temperHoseToBarrel_2;			//температура всасывающего шланга - 2		hoseToBarrel
	float 				temperHoseToReactor_2;			//температура напорного шланга - 2
	float 				temperReactorTop;			      //температура верхней части реактора
	float 				temperReactorMid;			      //температура средней части реактора
	float 				temperReactorLow;			      //температура нижней части реактора
	float					tempTarg;										// уставка
	float					tempCurr;									// действующая температура - минимальная из всех (того кто дольше нагревается)
	float 				currVolSumm;	//суммарный объем
	float					currVol_A;		//объем компонента А
	float					currVol_B;		//объем компонента Б
	float		 			SpeedCF_1;	// скорость вращения двигателя №1	(всасывающий)
	float				  SpeedCF_2;	// скорость вращения двигателя №2 (напорный)
	float				  SpeedCF_3;	// скорость вращения двигателя №3 (вакматор)
	float			    currWorkTime;	// время в режиме РАБОТА
	MachineState	statusFC_1;		// статус режима работы ПЧ-1	(всасывающий)
	MachineState	statusFC_2;		// статус режима работы ПЧ-2	(напорный)
	MachineState	statusFC_3;		// статус режима работы ПЧ-3	(вакматор)
	StatusVar calibrState;		   	    // режим калибровки (выкл/вкл)
	uint8_t 	heaterState;	//статус нагревателя
	MachineState   msachineState;		//статус
}CurrState;	// CurrentState

typedef struct BaseSetts{
	float	targetTemp;	
	float tempAccuracy;
	float speedPump_1;		   //скорость подачи насоса-1
	float	speedPump_2;			 //скорость подачи насоса-2
	float     maxFillSpeed_componA;		// максим. скорость налива компонента А
	float     maxFillSpeed_componB;		// максим. скорость налива компонента Б
}BaseSetts;



	

#endif /* __INTERACTIONS_H */
