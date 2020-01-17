/* ****************************************************************************
 Name        : gpioCtrl.c
 Description : GPIO CONTROL FUNCTIONS
**************************************************************************** */
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "boards.h"
#include "gpioCtrl.h"
#include "stdFunc.h"

/* ****************************************************************************
	STATIC DEFINES
**************************************************************************** */

/* ****************************************************************************
	STATIC FUNCTIONS
**************************************************************************** */
static void flagInit(void);

/* ****************************************************************************
	STATIC WORKS
**************************************************************************** */
static bool _bPairingSw;
bool _bPairingSwBack;
static bool _bEarSw;
static bool _bBatteryLow;

/* ----------------------------------------------------------------------------
 Name        : void gpioInit(void)
 Argument    : nil
 Result      : nil
 Description : IO端子初期化処理
---------------------------------------------------------------------------- */
void gpioInit(void)
{
	_bEarSw = false;
	_bPairingSw = false;
	_bBatteryLow = false;

	// スイッチ用入力端子設定
	/* BATTERYLOWのみここで初期化。ペアリングスイッチと耳たぶスイッチは、起床が必要な為
	   ライブラリ利用にて初期化
	   */
	nrf_gpio_cfg_input(DEF_INPORT_BATTERRY_LOW, NRF_GPIO_PIN_PULLUP);
	flagInit();
}

/* ----------------------------------------------------------------------------
 Name        : void flagInit(void)
 Argument    : nil
 Result      : nil
 Description : 各種フラグ初期化
---------------------------------------------------------------------------- */
static void flagInit(void)
{
	if (!(_bPairingSwBack = _bPairingSw = isPairingSw())) _bEarSw = true;
}

/* ----------------------------------------------------------------------------
 Name        : void gpioDebug(bool b)
 Argument    : nil
 Result      : nil
 Description : debug io control
---------------------------------------------------------------------------- */
static bool _bDebug = false;
void gpioDebug(bool b)
{
	if (b) {
		nrf_gpio_pin_write(DEF_OUTPORT_DEBUG, 1);
	}
	else {
		nrf_gpio_pin_write(DEF_OUTPORT_DEBUG, 0);
	}
	_bDebug = b;
}

/* ----------------------------------------------------------------------------
 Name        : void gpioPower(bool b)
 Argument    : nil
 Result      : nil
 Description : power io control
---------------------------------------------------------------------------- */
void gpioDebugToggle(void)
{
	if (_bDebug) {
		nrf_gpio_pin_write(DEF_OUTPORT_DEBUG, 1);
	}
	else {
		nrf_gpio_pin_write(DEF_OUTPORT_DEBUG, 0);
	}
	_bDebug^=0x01;
}

/* ----------------------------------------------------------------------------
 Name        : void gpioDebug2(bool b)
 Argument    : nil
 Result      : nil
 Description : power io control
---------------------------------------------------------------------------- */
static bool _bDebug2 = false;
void gpioDebug2(bool b)
{
	if (b) {
		nrf_gpio_pin_write(DEF_OUTPORT_POWER_ON, 1);
	}
	else {
		nrf_gpio_pin_write(DEF_OUTPORT_POWER_ON, 0);
	}
	_bDebug2 = b;
}

/* ----------------------------------------------------------------------------
 Name        : void gpioPower(bool b)
 Argument    : nil
 Result      : nil
 Description : power io control
---------------------------------------------------------------------------- */
void gpioDebugToggle2(void)
{
	if (_bDebug2) {
		nrf_gpio_pin_write(DEF_OUTPORT_POWER_ON, 1);
	}
	else {
		nrf_gpio_pin_write(DEF_OUTPORT_POWER_ON, 0);
	}
	_bDebug2^=0x01;
}

/* ----------------------------------------------------------------------------
 Name        : void gpioPower(bool b)
 Argument    : nil
 Result      : nil
 Description : power io control
---------------------------------------------------------------------------- */
void gpioPower(bool b)
{
	if (b) {
		nrf_gpio_pin_write(DEF_OUTPORT_POWER_ON, 1);
	}
	else {
		nrf_gpio_pin_write(DEF_OUTPORT_POWER_ON, 0);
	}
}

/* ----------------------------------------------------------------------------
 Name        : bool isPairingSw(void)
 Argument    : nil
 Result      : true	false
 Description : ペアリングスイッチの現在状態を戻り値に返します。
---------------------------------------------------------------------------- */
bool isPairingSw(void)
{
	uint16_t i;
	uint8_t b = 1;

	for (i=0; i<5; i++) {
		b &= (nrf_gpio_pin_read(DEF_INPORT_PAIRING) ^ 0x01);
		wait(10);
	}
	b &= (nrf_gpio_pin_read(DEF_INPORT_PAIRING) ^ 0x01);
	return b;
}

/* ----------------------------------------------------------------------------
 Name        : bool isBatteryLow(void)
 Argument    : nil
 Result      : true	false
 Description : バッテリローの現在状態を戻り値に返します。
---------------------------------------------------------------------------- */
bool isBatteryLow(void)
{
	return _bBatteryLow;
}

/* ----------------------------------------------------------------------------
 Name        : void readBatteryLow(void)
 Argument    : nil
 Result      : true	false
 Description : バッテリローの現在状態を取得します。
---------------------------------------------------------------------------- */
void readBatteryLow(void)
{
	_bBatteryLow = (nrf_gpio_pin_read(DEF_INPORT_BATTERRY_LOW) ^ 0x01);
	
	if (_bBatteryLow) {
		_bBatteryLow = false;
	}
	
}

/* ----------------------------------------------------------------------------
 Name        : bool isWakeupEarSw(void)
 Argument    : nil
 Result      : true	false
 Description : 耳たぶスイッチでの起床状態を戻り値に返します。
---------------------------------------------------------------------------- */
bool isWakeupEarSw(void)
{
	return _bEarSw;
}

/* ----------------------------------------------------------------------------
 Name        : bool isWakeupPairingSw(void)
 Argument    : nil
 Result      : true	false
 Description : ペアリングスイッチでの起床状態を戻り値に返します。
---------------------------------------------------------------------------- */
bool isWakeupPairingSw(void)
{
	return _bPairingSw;
}

/* ----------------------------------------------------------------------------
 Name        : void isPairingSwOff(void)
 Argument    : nil
 Result      : nil
 Description : ペアリングスイッチでの起床フラグをfalseにします。
---------------------------------------------------------------------------- */
void isPairingSwOff(void)
{
	_bPairingSw = false;
}

/* ----------------------------------------------------------------------------
 Name        : void GPIOTE_IRQHandler(void)
 Argument    : nil
 Result      : nil
 Description : スイッチ割込みハンドラ
---------------------------------------------------------------------------- */
void GPIOTE_IRQHandler(void)
{
	bool bPairingSw;

	if(NRF_GPIOTE->EVENTS_PORT) {
		//ここでイベントフラグをクリアしないと、無限に外部割込み処理を呼び続けます。
		NRF_GPIOTE->EVENTS_PORT = 0;

		/* 割り込み要因は、耳たぶスイッチの変化と、ペアリングスイッチのON/OFFのみなので、
		   ペアリングスイッチ押下、ペアリングスイッチ押下状態からのOFF 以外は耳たぶスイッチに
		   よる起床と判断する。
		*/
		bPairingSw = isPairingSw();

		if (bPairingSw) {	// ペアリングスイッチ押した
			_bPairingSw = true;
		}
	}
}
