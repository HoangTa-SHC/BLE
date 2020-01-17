/* ****************************************************************************
 Name        : timerCtrl.c
 Description : TIMER CONTROL FUNCTIONS
**************************************************************************** */
#include <string.h>
#include "boards.h"
#include "app_timer.h"
#include "timerCtrl.h"
#include "gpioCtrl.h"
#include "sensorCtrl.h"

/* ****************************************************************************
	STATIC DEFINES
**************************************************************************** */

/* ****************************************************************************
	STATIC FUNCTIONS
**************************************************************************** */
static void timerTimeoutHandler(void *pContext);

/* ****************************************************************************
	STATIC WORKS
**************************************************************************** */
APP_TIMER_DEF(_timerInstance);
static int32_t _lInactiveTime;
static uint16_t _unTimeAdv;
static uint32_t _ulTimeoutMsecBase;
static uint32_t _unTimeAdvDataExchg;
static bool _bInactiveEn;
static bool _bTimeAdvEn;
static bool _bTimeAdvDataExchgEn;

/* ----------------------------------------------------------------------------
 Name        : void timerInit(void)
 Argument    : nil
 Result      : nil
 Description : ������
---------------------------------------------------------------------------- */
void timerInit(void)
{
	_lInactiveTime = 0;
	_unTimeAdv = 0;
	_bInactiveEn = false;
	_bTimeAdvEn = false;
	_bTimeAdvDataExchgEn = false;

	void (*funcp)(void *);
	ret_code_t err_code = app_timer_init();
	APP_ERROR_CHECK(err_code);
	funcp = timerTimeoutHandler;
	err_code = app_timer_create(&_timerInstance, APP_TIMER_MODE_REPEATED, funcp);
	APP_ERROR_CHECK(err_code);
}

/* ----------------------------------------------------------------------------
 Name        : void timerStart(uint32_t timeout_ticks)
 Argument    : uint32_t timeout_ticks	���荞�݃`�b�N��
 Result      : nil
 Description : �w��̃`�b�N���Ŋ��荞�݂𔭐�����^�C�}�[���J�n���܂��B
---------------------------------------------------------------------------- */
void timerStart(uint32_t timeout_ticks)
{
	ret_code_t err_code;
	err_code = app_timer_start(_timerInstance, timeout_ticks, NULL);
	APP_ERROR_CHECK(err_code);
}

/* ----------------------------------------------------------------------------
 Name        : void timerSetWakeup(uint32_t timeout_ticks)
 Argument    : uint32_t timeout_ticks	�N���܂ł̃`�b�N��
 Result      : nil
 Description : �N���^�C�~���O���w��̃`�b�N���ɕύX���܂��B
---------------------------------------------------------------------------- */
void timerSetWakeup(uint32_t ulTimeoutMsec)
{
	uint32_t ulTimeoutTicks = (ulTimeoutMsec * DEF_TIMER_BASE_CLOCK) / 1000;

	// ���荞�݃^�C�~���O�C���^�[�o�����Ԑݒ�
	_ulTimeoutMsecBase = ulTimeoutMsec;
	ret_code_t err_code = app_timer_stop(_timerInstance);
	APP_ERROR_CHECK(err_code);
	timerStart(ulTimeoutTicks);
}

/* ----------------------------------------------------------------------------
 Name        : void timerStartAdv(uint16_t unSec)
 Argument    : uint16_t unSec	�^�C�}�[���� �P��:sec
 Result      : nil
 Description : �A�h�o�^�C�W���O�^�C�}�[�X�^�[�g
---------------------------------------------------------------------------- */
void timerStartAdv(uint16_t unSec)
{
//	app_timer_pause();
	_unTimeAdv = ((unSec * 1000) / _ulTimeoutMsecBase);
	_bTimeAdvEn= false;
//	app_timer_resume();
}

/* ----------------------------------------------------------------------------
 Name        : bool timerIsAdv(void)
 Argument    : nil
 Result      : nil
 Description : �A�h�o�^�C�W���O�^�C�}�[�^�C���A�b�v�m�F
---------------------------------------------------------------------------- */
bool timerIsAdv(void)
{
	bool r;
//	app_timer_pause();
	r = _bTimeAdvEn;
	_bTimeAdvEn = false;
//	app_timer_resume();
	return r;
}

/* ----------------------------------------------------------------------------
 Name        : void timerStartAdvDataExchg(uint16_t unMsec)
 Argument    : uint16_t unMsec	�^�C�}�[���� �P��:msec
 Result      : nil
 Description : �A�h�o�^�C�W���O�f�[�^�����^�C�}�[�}�[�X�^�[�g
---------------------------------------------------------------------------- */
void timerStartAdvDataExchg(uint16_t unMsec)
{
//	app_timer_pause();
	_unTimeAdvDataExchg = (unMsec / _ulTimeoutMsecBase);
	_bTimeAdvDataExchgEn= false;
//	app_timer_resume();
}

/* ----------------------------------------------------------------------------
 Name        : bool timerIsAdvDataExchg(void)
 Argument    : nil
 Result      : nil
 Description : �A�h�o�^�C�W���O�f�[�^�����^�C�}�[�^�C���A�b�v�m�F
---------------------------------------------------------------------------- */
bool timerIsAdvDataExchg(void)
{
	bool r;
//	app_timer_pause();
	r = _bTimeAdvDataExchgEn;
	_bTimeAdvDataExchgEn = false;
//	app_timer_resume();
	return r;
}

/* ----------------------------------------------------------------------------
 Name        : void timerStartInactive(void)
 Argument    : nil
 Result      : nil
 Description : ������~�^�C�}�[�X�^�[�g
---------------------------------------------------------------------------- */
void timerStartInactive(void)
{
//	app_timer_pause();
	_lInactiveTime = DEF_TIMER_INACTIVE_TIME;
	_bInactiveEn = false;
//	app_timer_resume();
}

/* ----------------------------------------------------------------------------
 Name        : void timerIsInactive(void)
 Argument    : nil
 Result      : nil
 Description : ������~�^�C�}�[�^�C���A�b�v�m�F
---------------------------------------------------------------------------- */
bool timerIsInactive(void)
{
	bool r;
//	app_timer_pause();
	r = _bInactiveEn;
	_bInactiveEn = false;
//	app_timer_resume();
//        if (r) {
//          r = true;
//        }
	return r;
}

/* ----------------------------------------------------------------------------
 Name        : void timerStopInactive(void)
 Argument    : nil
 Result      : nil
 Description : ������~�^�C�}�[���~���܂��B
---------------------------------------------------------------------------- */
void timerStopInactive(void)
{
//	app_timer_pause();
	_lInactiveTime = 0;
	_bInactiveEn = false;
//	app_timer_resume();
}

/* ----------------------------------------------------------------------------
 Name        : void timerTimeoutHandler(void *pContext)
 Argument    : nil
 Result      : nil
 Description : ������
---------------------------------------------------------------------------- */
static void timerTimeoutHandler(void *pContext)
{
	// �A�h�o�^�C�W���O�^�C�}�[
	if (_unTimeAdv > 0) {
		_unTimeAdv--;
		if (_unTimeAdv==0) {
			_bTimeAdvEn = true;
		}
	}

	// �A�h�o�^�C�W���O�f�[�^�؂�ւ��^�C�}�[
	if (_unTimeAdvDataExchg > 0) {
		_unTimeAdvDataExchg--;
		if (_unTimeAdvDataExchg==0) {
			_bTimeAdvDataExchgEn = true;
		}
	}

	// �����^�C�}�[
	if (_lInactiveTime > 0) {
		_lInactiveTime -= _ulTimeoutMsecBase;
		if (_lInactiveTime<=0) {
			_bInactiveEn = true;
		}
	}
}
