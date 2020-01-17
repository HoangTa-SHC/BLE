/* ****************************************************************************
 Name        : i2cCtrl.c
 Description : GPIO CONTROL FUNCTIONS
**************************************************************************** */
#define TWIM_IN_USE 1
#include <stdio.h>
#include "boards.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "nrf_drv_twi.h"

#include "sensorCtrl.h"
#include "gpioCtrl.h"
#include "adCtrl.h"
#include "i2cCtrl.h"

/* ****************************************************************************
	STATIC DEFINES
**************************************************************************** */

/* ****************************************************************************
	STATIC FUNCTIONS
**************************************************************************** */

/* ****************************************************************************
	STATIC WORKS
**************************************************************************** */
/* TWI instance. */

/* ----------------------------------------------------------------------------
 Name        : void sensorInit(void)
 Argument    : nil
 Result      : nil
 Description : sensor初期化処理
---------------------------------------------------------------------------- */
void sensorInit(void)
{
	adInit();
	i2cInit();
}

/* ----------------------------------------------------------------------------
 Name        : void sensorInit2(void)
 Argument    : nil
 Result      : nil
 Description : sensor初期化処理
---------------------------------------------------------------------------- */
void sensorInit2(void)
{
	adInit();
	initHdc2010();
	initLsm303();
}

/* ----------------------------------------------------------------------------
 Name        : void sensorRead(enum enSensor enType)
 Argument    : enum enSensor enSensor
 Result      : nil
 Description : HDC2010 湿度データ読出し
---------------------------------------------------------------------------- */
bool sensorRead(enum EN_SENSOR enType)
{
	bool r = true;

	switch (enType) {
	case EN_SENSOR_TEMP:	adRead();		break;
	case EN_SENSOR_ACCEL:	lsm303read();	break;
	case EN_SENSOR_HUM:		hdc2010read();	break;
	default:				r = false;		break;
	}
	return r;
}

/* ----------------------------------------------------------------------------
 Name        : bool sensorGetData(enum enSensor enType, uint16_t *punData)
 Argument    : enum enSensor enType	処理対象種別
   			   uint16_t *punData	取得したデータを格納するバッファポインタ
 Result      : true	false
 Description : データ読出し
---------------------------------------------------------------------------- */
bool sensorGetData(enum EN_SENSOR enType, uint16_t *punData)
{
	bool r = true;

	switch (enType) {
	case EN_SENSOR_TEMP:	r = adGetData(punData);			break;
	case EN_SENSOR_ACCEL:	r = i2cAccelGetData((int16_t*)punData);	break;
	case EN_SENSOR_HUM:		r = i2cHumGetData(punData);		break;
	default:				r = false;						break;
	}
	return r;
}
