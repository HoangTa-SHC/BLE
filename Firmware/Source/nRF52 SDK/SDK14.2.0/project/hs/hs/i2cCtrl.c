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
#include "gpioCtrl.h"
#include "i2cCtrl.h"
#include "stdFunc.h"

/* ****************************************************************************
	STATIC DEFINES
**************************************************************************** */
#define TWI_INSTANCE_ID	0

/* ****************************************************************************
	STATIC FUNCTIONS
**************************************************************************** */
static bool lsm303IsReady(void);
static bool hdc2010IsReady(void);
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context);

/* ****************************************************************************
	STATIC WORKS
**************************************************************************** */
/* TWI instance. */
const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);
static int16_t _nXyz[3];
static uint16_t _unHum;
static bool _bXyzEnable;
static bool _bHumEnable;



/* ----------------------------------------------------------------------------
 Name        : void lsm303read(void)
 Argument    : nil
 Result      : nil
 Description : LSM303 加速度読出し
   				読み出したデータは10ビット左詰め2の補数です。
---------------------------------------------------------------------------- */
void lsm303read2(uint8_t uAddr, int nLen, uint8_t a[])
{
	uint8_t uBuff[16];
	uint8_t uReg = uAddr;
	ret_code_t err_code;
	int i;

//	for (int i=0; i<5; i++) {
//		while (!lsm303IsReady());
//	}
	err_code = nrf_drv_twi_txrx(&m_twi, DEF_ADDR_LSM303, &uReg, 1, (uint8_t*)uBuff, nLen);
	APP_ERROR_CHECK(err_code);

	for (i=0;i<nLen;i++){
		a[i] = *(uint16_t*)&uBuff[i];	// x
	}
}

int32_t unXyz3[3];
uint8_t a=0xd4;
void i2cSensor(void)
{
	ret_code_t err_code;
	uint8_t uSlaveAddress = DEF_ADDR_LSM303;
	uint8_t uData[0x40];
	int32_t unXyz1[3];
	int32_t unXyz2[3];
//	int32_t unXyz3[3];
	int i;

//	i2cInit();

	wait(DEF_STDFUNC_1MSEC_BASE * 90);	// LSM303 READY WAIT 90msec
//	lsm303IsReady();		// read status(0x27)

	for (i=0; i<100000; i++) {
		lsm303read();
	}
	
	
//	while (lsm303IsReady()) {
		lsm303read();	// celar
//		lsm303read2(0,2);
//		lsm303read2(0x1f,10);
//		lsm303read2(0x28,2);
//	}

	unXyz1[0] = 0;
	unXyz1[1] = 0;
	unXyz1[2] = 0;
	for (i=0; i<5; i++) {
		lsm303read();
		unXyz1[0] += (int16_t)(_nXyz[0]) * -1;
		unXyz1[1] += (int16_t)(_nXyz[1]) * -1;
		unXyz1[2] += (int16_t)(_nXyz[2]) * -1;
	}
	unXyz1[0] /= 5;
	unXyz1[1] /= 5;
	unXyz1[2] /= 5;

	// REG 0x23
	uData[0] = DEF_LMH303_CTRL_REG4;
	uData[1] = 0x85;
	err_code = nrf_drv_twi_tx(&m_twi, uSlaveAddress, &uData[0], 2, false);
	APP_ERROR_CHECK(err_code);
	wait(DEF_STDFUNC_1MSEC_BASE * 90);	// LSM303 READY WAIT 90msec

	lsm303read();	// celar

	unXyz2[0] = 0;
	unXyz2[1] = 0;
	unXyz2[2] = 0;
	for (i=0; i<5; i++) {
		lsm303read();
		unXyz2[0] += (int16_t)(_nXyz[0]) * -1;
		unXyz2[1] += (int16_t)(_nXyz[1]) * -1;
		unXyz2[2] += (int16_t)(_nXyz[2]) * -1;
	}
	unXyz2[0] /= 5;
	unXyz2[1] /= 5;
	unXyz2[2] /= 5;

	unXyz3[0] = unXyz2[0] - unXyz1[0];
	unXyz3[1] = unXyz2[1] - unXyz1[1];
	unXyz3[2] = unXyz2[2] - unXyz1[2];

	// REG 0x23
	uData[0] = DEF_LMH303_CTRL_REG1;
	uData[1] = 0x00;
	err_code = nrf_drv_twi_tx(&m_twi, uSlaveAddress, &uData[0], 2, false);
	APP_ERROR_CHECK(err_code);

	// REG 0x23
	uData[0] = DEF_LMH303_CTRL_REG4;
	uData[1] = 0x01;
	err_code = nrf_drv_twi_tx(&m_twi, uSlaveAddress, &uData[0], 2, false);
	APP_ERROR_CHECK(err_code);

	wait(DEF_STDFUNC_1MSEC_BASE * 90);	// Ready wait 90msec
}

/* ----------------------------------------------------------------------------
 Name        : void i2cInit(void)
 Argument    : nil
 Result      : nil
 Description : i2c初期化処理
---------------------------------------------------------------------------- */
void i2cInit(void)
{
	ret_code_t err_code;

	const nrf_drv_twi_config_t twi_config = {
		.scl                = DEF_I2C_SCL_PIN,
		.sda                = DEF_I2C_SDA_PIN,
		.frequency          = NRF_TWI_FREQ_100K,
		.interrupt_priority = APP_IRQ_PRIORITY_HIGH,
		.clear_bus_init     = false
	};
	_nXyz[0] = 0;
	_nXyz[1] = 0;
	_nXyz[2] = 0;
	_bXyzEnable = false;
	_bHumEnable = false;
	err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
	APP_ERROR_CHECK(err_code);
	nrf_drv_twi_enable(&m_twi);

	initHdc2010();
	initLsm303();
	wait(DEF_STDFUNC_1MSEC_BASE * 90);	// LSM303 READY WAIT 90msec
}

/* ----------------------------------------------------------------------------
 Name        : static void initHdc2010(void)
 Argument    : nil
 Result      : nil
 Description : LSM303 加速度読出し
---------------------------------------------------------------------------- */
void initHdc2010(void)
{
/*
	   ret_code_t err_code;
	uint8_t uSlaveAddress = DEF_ADDR_HDC2010;
	uint8_t uData[2];

	uData[0] = DEF_HDC2010_CONF_ADDR;
	uData[1] = DEF_HDC2010_CONF_DATA;
	err_code = nrf_drv_twi_tx(&m_twi, uSlaveAddress, &uData[0], 2, false);
	APP_ERROR_CHECK(err_code);

	uData[0] = 0x09;
	uData[1] = 0xc0;
	err_code = nrf_drv_twi_tx(&m_twi, uSlaveAddress, &uData[0], 2, false);
	APP_ERROR_CHECK(err_code);

	hdc2010read();	// から読み
*/
}

/* ----------------------------------------------------------------------------
 Name        : void initLsm303(void)
 Argument    : nil
 Result      : nil
 Description : LSM303 加速度読出し
---------------------------------------------------------------------------- */
void initLsm303(void)
{
	ret_code_t err_code;
	uint8_t uSlaveAddress = DEF_ADDR_LSM303;
	uint8_t uData[2];

	// REG 0x21
	uData[0] = DEF_LMH303_CTRL_REG2;
	uData[1] = DEF_LMH303_CTRL_REG2_DATA;
	err_code = nrf_drv_twi_tx(&m_twi, uSlaveAddress, &uData[0], 2, false);
	APP_ERROR_CHECK(err_code);

	// REG 0x22
	uData[0] = DEF_LMH303_CTRL_REG3;
	uData[1] = DEF_LMH303_CTRL_REG3_DATA;
	err_code = nrf_drv_twi_tx(&m_twi, uSlaveAddress, &uData[0], 2, false);
	APP_ERROR_CHECK(err_code);

	// REG 0x23
	uData[0] = DEF_LMH303_CTRL_REG4;
	uData[1] = DEF_LMH303_CTRL_REG4_DATA;
	err_code = nrf_drv_twi_tx(&m_twi, uSlaveAddress, &uData[0], 2, false);
	APP_ERROR_CHECK(err_code);

	// REG 0x20
	uData[0] = DEF_LMH303_CTRL_REG1;
	uData[1] = DEF_LMH303_CTRL_REG1_DATA;
	err_code = nrf_drv_twi_tx(&m_twi, uSlaveAddress, &uData[0], 2, false);
	APP_ERROR_CHECK(err_code);
}

/* ----------------------------------------------------------------------------
 Name        : bool i2cAccelGetData(uint16_t unXyz[])
 Argument    : uint16_t unXyz[]	データ格納バッファ
 Result      : true	false
 Description : 加速度値引き渡し
---------------------------------------------------------------------------- */
bool i2cAccelGetData(int16_t nXyz[])
{
	if (_bXyzEnable) {
		nXyz[0] = _nXyz[0];
		nXyz[1] = _nXyz[1];
		nXyz[2] = _nXyz[2];
		_bXyzEnable = false;
		return true;
	}
	return false;
}

/* ----------------------------------------------------------------------------
 Name        : bool i2cHumGetData(uint16_t *punHum)
 Argument    : uint16_t *punHum	データ格納バッファ
 Result      : true	false
 Description : 湿度値引き渡し
---------------------------------------------------------------------------- */
bool i2cHumGetData(uint16_t *punHum)
{
	if (_bHumEnable) {
		*punHum = _unHum;
		_bHumEnable = false;
		return true;
	}
	return false;
}

/* ----------------------------------------------------------------------------
 Name        : static void hdc2010read(void)
 Argument    : nil
 Result      : nil
 Description : HDC2010 湿度データ読出し
---------------------------------------------------------------------------- */
void hdc2010read(void)
{
	ret_code_t err_code;
	uint8_t uReg = DEF_HDC2010_HUML_ADDR;
	uint8_t uBuff[2];
	uint8_t uData[2];

	// start measurement
	uData[0] = DEF_HDC2010_CONF_ADDR;
	uData[1] = DEF_HDC2010_CONF_DATA;
	err_code = nrf_drv_twi_tx(&m_twi, DEF_ADDR_HDC2010, &uData[0], 2, false);
	APP_ERROR_CHECK(err_code);

	while (!hdc2010IsReady());

	err_code = nrf_drv_twi_txrx(&m_twi, DEF_ADDR_HDC2010, &uReg, 1, uBuff, 2);
	APP_ERROR_CHECK(err_code);
	_unHum = *(uint16_t*)uBuff;
	_unHum &= 0xffe0;	// 11bit mask
	_bHumEnable = true;
}

/* ----------------------------------------------------------------------------
 Name        : void lsm303read(void)
 Argument    : nil
 Result      : nil
 Description : LSM303 加速度読出し
   				読み出したデータは10ビット左詰め2の補数です。
---------------------------------------------------------------------------- */
void lsm303read(void)
{
	int16_t nXyz[3];
	uint8_t uBuff[10];
	uint8_t uReg = DEF_LSM303_XH_ADDR|DEF_LSM303_ADDRINC;
	ret_code_t err_code;

	while (!lsm303IsReady());

	err_code = nrf_drv_twi_txrx(&m_twi, DEF_ADDR_LSM303, &uReg, 1, (uint8_t*)uBuff, 6);
	APP_ERROR_CHECK(err_code);

	nXyz[0] = *(uint16_t*)&uBuff[0];	// x
	nXyz[1] = *(uint16_t*)&uBuff[2];	// z
	nXyz[2] = *(uint16_t*)&uBuff[4];	// y

	_nXyz[0] = nXyz[0];
	_nXyz[1] = nXyz[1];
	_nXyz[2] = nXyz[2];

	_bXyzEnable = true;
}

/* ----------------------------------------------------------------------------
 Name        : bool hdc2010IsReady(void)
 Argument    : nil
 Result      : true	false
 Description : LSM303 データREADY 確認
---------------------------------------------------------------------------- */
static bool hdc2010IsReady(void)
{
	ret_code_t err_code;
	uint8_t uReg = DEF_HDC2010_STATUS_ADDR;
	uint8_t uStatus;

	err_code = nrf_drv_twi_txrx(&m_twi, DEF_ADDR_HDC2010, &uReg, 1, &uStatus, 1);
	APP_ERROR_CHECK(err_code);

	if ((uStatus & DEF_HDC2010_STATUS_READY)!=0)
		return true;

	return false;
}

/* ----------------------------------------------------------------------------
 Name        : bool lsm303IsReady(void)
 Argument    : nil
 Result      : true	false
 Description : LSM303 データREADY 確認
---------------------------------------------------------------------------- */
static bool lsm303IsReady(void)
{
	ret_code_t err_code;
	uint8_t uReg = DEF_LMH303_REG_STATUS;
	uint8_t uStatus;

	err_code = nrf_drv_twi_txrx(&m_twi, DEF_ADDR_LSM303, &uReg, 1, &uStatus, 1);
	APP_ERROR_CHECK(err_code);

	if ((uStatus & DEF_LMH303_REG_STATUS_READY)!=0)
		return true;

	return false;
}

/**
 * @brief Function for handling data from temperature sensor.
 *
 * @param[in] temp          Temperature in Celsius degrees read from sensor.
 */
/*
__STATIC_INLINE void data_handler(uint8_t temp)
{
    NRF_LOG_INFO("Temperature: %d Celsius degrees.", temp);
}
*/
/* YOUR_JOB: Declare all services structure your application is using
 *  BLE_XYZ_DEF(m_xyz);
 */
/**
 * @brief TWI events handler.
 */
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
	switch (p_event->type) {
	case NRF_DRV_TWI_EVT_DONE:
		if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX) {
			//data_handler(m_sample);
		}
		//m_xfer_done = true;
		break;
	default: break;
	}
}

