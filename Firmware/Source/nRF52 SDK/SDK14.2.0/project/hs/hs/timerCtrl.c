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
 Description : 初期化
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
 Argument    : uint32_t timeout_ticks	割り込みチック数
 Result      : nil
 Description : 指定のチック数で割り込みを発生するタイマーを開始します。
---------------------------------------------------------------------------- */
void timerStart(uint32_t timeout_ticks)
{
	ret_code_t err_code;
	err_code = app_timer_start(_timerInstance, timeout_ticks, NULL);
	APP_ERROR_CHECK(err_code);
}

/* ----------------------------------------------------------------------------
 Name        : void timerSetWakeup(uint32_t timeout_ticks)
 Argument    : uint32_t timeout_ticks	起床までのチック数
 Result      : nil
 Description : 起床タイミングを指定のチック数に変更します。
---------------------------------------------------------------------------- */
void timerSetWakeup(uint32_t ulTimeoutMsec)
{
	uint32_t ulTimeoutTicks = (ulTimeoutMsec * DEF_TIMER_BASE_CLOCK) / 1000;

	// 割り込みタイミングインターバル時間設定
	_ulTimeoutMsecBase = ulTimeoutMsec;
	ret_code_t err_code = app_timer_stop(_timerInstance);
	APP_ERROR_CHECK(err_code);
	timerStart(ulTimeoutTicks);
}

/* ----------------------------------------------------------------------------
 Name        : void timerStartAdv(uint16_t unSec)
 Argument    : uint16_t unSec	タイマー時間 単位:sec
 Result      : nil
 Description : アドバタイジングタイマースタート
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
 Description : アドバタイジングタイマータイムアップ確認
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
 Argument    : uint16_t unMsec	タイマー時間 単位:msec
 Result      : nil
 Description : アドバタイジングデータ交換タイマーマースタート
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
 Description : アドバタイジングデータ交換タイマータイムアップ確認
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
 Description : 活動停止タイマースタート
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
 Description : 活動停止タイマータイムアップ確認
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
 Description : 活動停止タイマーを停止します。
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
 Description : 初期化
---------------------------------------------------------------------------- */
static void timerTimeoutHandler(void *pContext)
{
	// アドバタイジングタイマー
	if (_unTimeAdv > 0) {
		_unTimeAdv--;
		if (_unTimeAdv==0) {
			_bTimeAdvEn = true;
		}
	}

	// アドバタイジングデータ切り替えタイマー
	if (_unTimeAdvDataExchg > 0) {
		_unTimeAdvDataExchg--;
		if (_unTimeAdvDataExchg==0) {
			_bTimeAdvDataExchgEn = true;
		}
	}

	// 活動タイマー
	if (_lInactiveTime > 0) {
		_lInactiveTime -= _ulTimeoutMsecBase;
		if (_lInactiveTime<=0) {
			_bInactiveEn = true;
		}
	}
}
