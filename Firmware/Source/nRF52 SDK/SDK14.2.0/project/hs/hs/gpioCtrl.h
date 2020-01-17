/* ****************************************************************************
 Name        : gpioCtrl.c
 Description : GPIO CONTROL FUNCTIONS
**************************************************************************** */
#include "nrf_gpio.h"
#include "boards.h"

/* ****************************************************************************
	DEFINES
**************************************************************************** */
#define	DEF_INPORT_PAIRING		6			// 押下でLo
#define	DEF_INPORT_BATTERRY_LOW	14
#define	DEF_INPORT_EAR			17			// カチッってなるスイッチ
#define	DEF_OUTPORT_DEBUG		7
#define	DEF_OUTPORT_DEBUG2		40
#define	DEF_OUTPORT_POWER_ON	13

/* ****************************************************************************
	FUNCTIONS
**************************************************************************** */
extern	void gpioInit(void);
extern	void gpioInterruptEnable(void);
extern	void gpioDebug(bool b);
extern	void gpioDebugToggle(void);
extern	void gpioDebug2(bool b);
extern	void gpioDebugToggle2(void);
extern	void gpioPower(bool b);
extern	bool isPairingSw(void);
extern	void readBatteryLow(void);
extern	bool isBatteryLow(void);
extern	bool isWakeupEarSw(void);
extern	bool isWakeupPairingSw(void);
extern	void isPairingSwOff(void);
extern	void GPIOTE_IRQHandler(void);
