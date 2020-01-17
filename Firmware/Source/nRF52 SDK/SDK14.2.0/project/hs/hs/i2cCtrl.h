/* ****************************************************************************
 Name        : i2cCtrl.c
 Description : ADC CONTROL FUNCTIONS
**************************************************************************** */
#ifndef	__I2CCTRL_H__
#define	__I2CCTRL_H__

#include "nrf_gpio.h"
#include "boards.h"

/* ****************************************************************************
	DEFINES
**************************************************************************** */
#define TWI_INSTANCE_ID     0	/* TWI instance ID. */

#define	DEF_I2C_SCL_PIN		8
#define	DEF_I2C_SDA_PIN		9


// HDC2010
// humidity address
#define	DEF_ADDR_HDC2010			0x40		// slave address

#define	DEF_HDC2010_STATUS_READY	0x80	// DataReady bit

#define	DEF_HDC2010_HUML_ADDR		0x02
#define	DEF_HDC2010_HUMH_ADDR		0x03
#define	DEF_HDC2010_STATUS_ADDR		0x04
#define	DEF_HDC2010_CONF_ADDR		0x0f		// HDC2010 CONTROL REGISTER
#define	DEF_HDC2010_CONF_DATA		0xd5		// D0:   0-> no action
												//       1-> measurement start
												// D1-2: 00-> Humidity+Temperature
												//       01-> Temperature only
												//       10-> Humidity only
												//       11-> Temperature only
												// D3:   0-> Reserved
												// D4-5: HUMIDITY RESOLUTION
												//       00-> 14bit
												//       01-> 11bit
												//       10-> 8bit
												//       11-> NA
												// D6-7: TEMPRATURE RESOLUTION
												//       00-> 14bit
												//       01-> 11bit
												//       10-> 8bit
												//       11-> NA


// LSM303 ‰Á‘¬“xŒv
#define	DEF_ADDR_LSM303				0x19	// write address
#define	DEF_LMH303_CTRL_REG1		0x20	// LMH303 CTRL REGISTER
#define	DEF_LMH303_CTRL_REG1_DATA	0x37	// LMH303 CTRL REGISTER DATA 25Hz,XYZ-en
/*
D4-D7:ODR
	Data rate selection. Default value: 0000
	(0000: power-down mode; others: refer to Table 35)
D3:LPen
	Low-power mode enable. Default value: 0
	(0: normal mode, 1: low-power mode)
	(Refer to Section 4.2.1: Accelerometer power modes)
D2:Zen
	Z-axis enable. Default value: 1
	(0: Z-axis disabled; 1: Z-axis enabled)
D1:Yen
	Y-axis enable. Default value: 1
	(0: Y-axis disabled; 1: Y-axis enabled)
D0:Xen
	X-axis enable. Default value: 1
	(0: X-axis disabled; 1: X-axis enabled)
   */
/* W */
#define	DEF_LMH303_CTRL_REG2		0x21	// LMH303 CTRL REGISTER2
#define	DEF_LMH303_CTRL_REG2_DATA	0x00	// LMH303 CTRL REGISTER DATA

/* W */
#define	DEF_LMH303_CTRL_REG3		0x22	// LMH303 CTRL REGISTER3	interrupt settig
#define	DEF_LMH303_CTRL_REG3_DATA	0x00	// LMH303 CTRL REGISTER3 DATA

/* W */
#define	DEF_LMH303_CTRL_REG4		0x23	// LMH303 CTRL REGISTER4
#define	DEF_LMH303_CTRL_REG4_DATA	0x10	// LMH303 CTRL REGISTER4 DATA	FS->}4g
/*
   1000 0000 --> 0x80
  
D7:BDU
   Block data update. Default value: 0
	(0: continuous update; 1: output registers not updated until MSB and LSB have been read)
D6:BLE
   Big/Little Endian data selection. Default value: 0
	(0: data LSb at lower address; 1: data MSb at lower address)
	The BLE function can be activated only in high-resolution mode
D4-5:FS[00->}2g]
   Full-scale selection. Default value: 00
	(00: }2g; 01: }4g; 10: }8g; 11: }16g)
D3:HR
   Operating mode selection (refer to Section 4.2.1: Accelerometer power modes)
D1-2:ST[00->10-bit]
   Self-test enable. Default value: 00
	(00: self-test disabled; other: see Table 43)
D0:SPI_ENABLE[0]
   3-wire SPI interface enable. Default: 0
	(0: SPI 3-wire disabled; 1: SPI 3-wire enabled)
 */

#define	DEF_LMH303_CTRL_REG5		0x24	// LMH303 CTRL REGISTER5	interrupt settig
#define	DEF_LMH303_CTRL_REG5_DATA	0x00	// LMH303 CTRL REGISTER5 DATA	default

#define	DEF_LMH303_CTRL_REG6		0x25	// LMH303 CTRL REGISTER6	int2 setting
#define	DEF_LMH303_CTRL_REG6_DATA	0x00	// LMH303 CTRL REGISTER6 DATA	default

#define	DEF_LMH303_REG_REF			0x26	// LMH303 CTRL REFERENCE/DATACAPTURE_A
#define	DEF_LMH303_REG_REF_DATA		0x00	// LMH303 CTRL STER6 DATA

/* R */
#define	DEF_LMH303_REG_STATUS		0x27	// LMH303 CTRL STATUS	READ
#define	DEF_LMH303_REG_STATUS_READY	0x08

/*
D7:ZYXOR
   X-, Y- and Z-axis data overrun. Default value: 0
	(0: no overrun has occurred; 1: a new set of data has overwritten the previous set)
D6:ZOR
   Z-axis data overrun. Default value: 0
	(0: no overrun has occurred; 1: new data for the Z-axis has overwritten the previous data)
D5:YOR
   Y-axis data overrun. Default value: 0
	(0: no overrun has occurred;
	1: new data for the Y-axis has overwritten the previous data)
D4:XOR
   X-axis data overrun. Default value: 0
	(0: no overrun has occurred;
	1: new data for the X-axis has overwritten the previous data)
D3:ZYXDA
   X-, Y- and Z-axis new data available. Default value: 0
	(0: a new set of data is not yet available; 1: a new set of data is available)
D2:ZDA
   Z-axis new data available. Default value: 0
	(0: new data for the Z-axis is not yet available;
	1: new data for the Z-axis is available)
D1:YDA
   Y-axis new data available. Default value: 0
	(0: new data for the Y-axis is not yet available;
	1: new data for the Y-axis is available)   
D0:XDA
   X-axis new data available. Default value: 0
	(0: new data for the X-axis is not yet available;
	1: new data for the X-axis is available)   
 */

#define	DEF_LSM303_XH_ADDR	0x28
#define	DEF_LSM303_XL_ADDR	0x29
#define	DEF_LSM303_ZH_ADDR	0x2a
#define	DEF_LSM303_ZL_ADDR	0x2b
#define	DEF_LSM303_YH_ADDR	0x2c
#define	DEF_LSM303_YL_ADDR	0x2d
#define	DEF_LSM303_ADDRINC	0x80

enum EN_PTYPE {
	EN_PTYPE_HDC2010,		// Ž¼“xƒZƒ“ƒT
	EN_PTYPE_LSM303,		// ‰Á‘¬“xƒZƒ“ƒT
	EN_PTYPE_MAX
};

/* ****************************************************************************
	FUNCTIONS
**************************************************************************** */
extern	void i2cSensor(void);
extern	void i2cInit(void);
extern	bool i2cAccelGetData(int16_t nXyz[]);
extern	bool i2cHumGetData(uint16_t *punHum);
extern	void hdc2010read(void);
extern	void lsm303read(void);
extern	void initHdc2010(void);
extern	void initLsm303(void);

#endif	// __I2CCTRL_H__
