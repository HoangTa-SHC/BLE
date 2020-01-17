/* ****************************************************************************
 Name        : adCtrl.c
 Description : ADC CONTROL FUNCTIONS
**************************************************************************** */
#ifndef	__ADCTRL_H__
#define	__ADCTRL_H__

#include "nrf_gpio.h"
#include "boards.h"

/* ****************************************************************************
	DEFINES
**************************************************************************** */
#define	DEF_ADC_CH0		0
#define	DEF_ADC_CH1		0
#define	DEF_ADC_PIN0	NRF_SAADC_INPUT_AIN2
#define	DEF_ADC_PIN1	NRF_SAADC_INPUT_AIN3

/* ****************************************************************************
	FUNCTIONS
**************************************************************************** */
extern	void adInit(void);
extern	bool adGetData(uint16_t punAdc[]);
extern	void adRead(void);
#endif	// __ADCTRL_H__
