#ifndef __SENSOR_CTRL_H__
#define __SENSOR_CTRL_H__
/* ****************************************************************************
 Name        : sensorCtrl.c
 Description : SENSOR CONTROL FUNCTIONS
**************************************************************************** */

/* ****************************************************************************
	DEFINES
**************************************************************************** */
enum EN_SENSOR {
	EN_SENSOR_TEMP,		// ‰·“x
	EN_SENSOR_HUM,		// Ž¼“x
	EN_SENSOR_ACCEL,	// ‰Á‘¬“x
	EN_SENSOR_MAX
};

/* ****************************************************************************
	FUNCTIONS
**************************************************************************** */
extern	void sensorInit(void);
extern	void sensorInit2(void);
extern	bool sensorRead(enum EN_SENSOR enType);
extern	bool sensorGetData(enum EN_SENSOR enType, uint16_t *punData);

#endif	// __SENSOR_CTRL_H__
