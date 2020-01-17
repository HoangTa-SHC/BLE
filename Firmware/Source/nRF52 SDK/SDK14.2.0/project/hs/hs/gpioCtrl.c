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
 Description : IO�[�q����������
---------------------------------------------------------------------------- */
void gpioInit(void)
{
	_bEarSw = false;
	_bPairingSw = false;
	_bBatteryLow = false;

	// �X�C�b�`�p���͒[�q�ݒ�
	/* BATTERYLOW�݂̂����ŏ������B�y�A�����O�X�C�b�`�Ǝ����ԃX�C�b�`�́A�N�����K�v�Ȉ�
	   ���C�u�������p�ɂď�����
	   */
	nrf_gpio_cfg_input(DEF_INPORT_BATTERRY_LOW, NRF_GPIO_PIN_PULLUP);
	flagInit();
}

/* ----------------------------------------------------------------------------
 Name        : void flagInit(void)
 Argument    : nil
 Result      : nil
 Description : �e��t���O������
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
 Description : �y�A�����O�X�C�b�`�̌��ݏ�Ԃ�߂�l�ɕԂ��܂��B
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
 Description : �o�b�e�����[�̌��ݏ�Ԃ�߂�l�ɕԂ��܂��B
---------------------------------------------------------------------------- */
bool isBatteryLow(void)
{
	return _bBatteryLow;
}

/* ----------------------------------------------------------------------------
 Name        : void readBatteryLow(void)
 Argument    : nil
 Result      : true	false
 Description : �o�b�e�����[�̌��ݏ�Ԃ��擾���܂��B
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
 Description : �����ԃX�C�b�`�ł̋N����Ԃ�߂�l�ɕԂ��܂��B
---------------------------------------------------------------------------- */
bool isWakeupEarSw(void)
{
	return _bEarSw;
}

/* ----------------------------------------------------------------------------
 Name        : bool isWakeupPairingSw(void)
 Argument    : nil
 Result      : true	false
 Description : �y�A�����O�X�C�b�`�ł̋N����Ԃ�߂�l�ɕԂ��܂��B
---------------------------------------------------------------------------- */
bool isWakeupPairingSw(void)
{
	return _bPairingSw;
}

/* ----------------------------------------------------------------------------
 Name        : void isPairingSwOff(void)
 Argument    : nil
 Result      : nil
 Description : �y�A�����O�X�C�b�`�ł̋N���t���O��false�ɂ��܂��B
---------------------------------------------------------------------------- */
void isPairingSwOff(void)
{
	_bPairingSw = false;
}

/* ----------------------------------------------------------------------------
 Name        : void GPIOTE_IRQHandler(void)
 Argument    : nil
 Result      : nil
 Description : �X�C�b�`�����݃n���h��
---------------------------------------------------------------------------- */
void GPIOTE_IRQHandler(void)
{
	bool bPairingSw;

	if(NRF_GPIOTE->EVENTS_PORT) {
		//�����ŃC�x���g�t���O���N���A���Ȃ��ƁA�����ɊO�������ݏ������Ăё����܂��B
		NRF_GPIOTE->EVENTS_PORT = 0;

		/* ���荞�ݗv���́A�����ԃX�C�b�`�̕ω��ƁA�y�A�����O�X�C�b�`��ON/OFF�݂̂Ȃ̂ŁA
		   �y�A�����O�X�C�b�`�����A�y�A�����O�X�C�b�`������Ԃ����OFF �ȊO�͎����ԃX�C�b�`��
		   ���N���Ɣ��f����B
		*/
		bPairingSw = isPairingSw();

		if (bPairingSw) {	// �y�A�����O�X�C�b�`������
			_bPairingSw = true;
		}
	}
}
