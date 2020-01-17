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
// �N�����Ԓ�`
enum EN_TIMER_WAJEUP {
	EN_TIMER_WAJEUP_50MS,	// 50msec�^�C�}�[
	EN_TIMER_WAJEUP_30S,	// 30�b�^�C�}�[
	EN_TIMER_WAJEUP_MAX
};

/*
   �^�C�}�[���荞�݂́A�W�����C�u���� app_timer.c �𗘗p���邪�A�{���W���[���� 32768Hz��RTC1�𗘗p���Ă���
   1sec 32768Hz�Œ�ƂȂ��Ă���B
   �^�C�}�[�����ꍇ�A�v���X�P�[���l(�����l)���`���邱�ƁB
   50msec���Ɋ��荞�݂���������B������x�[�X�Ɋe�^�C�}�[�������̂Ƃ���B

	APP_TIMER_CONFIG_RTC_FREQUENCY	((32768/10/2) + 1) 	// base timer prescale �� hs52810.h �ɂĒ�`
   */
#define	DEF_TIMER_BASE_CLOCK	32768	// BASE TICK COUNT PER 1SEC.
#define	DEF_TIMER_50MS_TICK		(DEF_TIMER_BASE_CLOCK / (1000 / 50))
#define	DEF_TIMER_100MS_TICK	(DEF_TIMER_50MS_TICK * 2)
#define	DEF_TIMER_1S_TICK		(DEF_TIMER_100MS_TICK * 10)
#define	DEF_TIMER_30S_TICK		(DEF_TIMER_1S_TICK * 30)
#define	DEF_TIMER_1MIN_TICK		(DEF_TIMER_30S_TICK * 2)

#define	DEF_TIMER_5S_TICK		(DEF_TIMER_1S_TICK * 5)		// 5sec wait test
//#define	DEF_TIMER_INACTIVE_TIME	(210*1000)	// ������~�^�C�}�[ 3��30�b
//#define	DEF_TIMER_INACTIVE_TIME	((180 - 3)*1000)	// ������~�^�C�}�[ 3��30�b
#define	DEF_TIMER_INACTIVE_TIME	((420-3)*1000)		// ������~�^�C�}�[ 7��

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
