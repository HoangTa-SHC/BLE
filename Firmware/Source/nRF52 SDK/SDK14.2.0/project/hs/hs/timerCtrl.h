/* ****************************************************************************
 Name        : adCtrl.c
 Description : ADC CONTROL FUNCTIONS
**************************************************************************** */
#ifndef	__TIMERCTRL_H__
#define	__TIMERCTRL_H__

#include "boards.h"

/* ****************************************************************************
	DEFINES
**************************************************************************** */
// 起床時間定義
enum EN_TIMER_WAJEUP {
	EN_TIMER_WAJEUP_50MS,	// 50msecタイマー
	EN_TIMER_WAJEUP_30S,	// 30秒タイマー
	EN_TIMER_WAJEUP_MAX
};

/*
   タイマー割り込みは、標準ライブリの app_timer.c を利用するが、本モジュールは 32768HzのRTC1を利用しており
   1sec 32768Hz固定となっている。
   タイマーを作る場合、プリスケール値(分周値)を定義すること。
   50msec毎に割り込みが発生する。それをベースに各タイマーを作るものとする。

	APP_TIMER_CONFIG_RTC_FREQUENCY	((32768/10/2) + 1) 	// base timer prescale は hs52810.h にて定義
   */
#define	DEF_TIMER_BASE_CLOCK	32768	// BASE TICK COUNT PER 1SEC.
#define	DEF_TIMER_50MS_TICK		(DEF_TIMER_BASE_CLOCK / (1000 / 50))
#define	DEF_TIMER_100MS_TICK	(DEF_TIMER_50MS_TICK * 2)
#define	DEF_TIMER_1S_TICK		(DEF_TIMER_100MS_TICK * 10)
#define	DEF_TIMER_30S_TICK		(DEF_TIMER_1S_TICK * 30)
#define	DEF_TIMER_1MIN_TICK		(DEF_TIMER_30S_TICK * 2)

#define	DEF_TIMER_5S_TICK		(DEF_TIMER_1S_TICK * 5)		// 5sec wait test
//#define	DEF_TIMER_INACTIVE_TIME	(210*1000)	// 活動停止タイマー 3分30秒
//#define	DEF_TIMER_INACTIVE_TIME	((180 - 3)*1000)	// 活動停止タイマー 3分30秒
#define	DEF_TIMER_INACTIVE_TIME	((420-3)*1000)		// 活動停止タイマー 7分

/* ****************************************************************************
	FUNCTIONS
**************************************************************************** */
extern	void timerInit(void);
extern	void timerSetWakeup(uint32_t timeout_ticks);
extern	void timerStartInactive(void);
extern	bool timerIsInactive(void);
extern	void timerStart(uint32_t timeout_ticks);
extern	void timerStopInactive(void);
extern	void timerStartAdv(uint16_t unSec);
extern	bool timerIsAdv(void);
extern	void timerStartAdvDataExchg(uint16_t unMsec);
extern	bool timerIsAdvDataExchg(void);

#endif	// __TIMERCTRL_H__
