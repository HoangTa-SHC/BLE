/* ****************************************************************************
 Name        : stdFunc.c
 Description : ADC CONTROL FUNCTIONS
**************************************************************************** */
#ifndef	__STDFUNC_H__
#define	__STDFUNC_H__

/* ****************************************************************************
	DEFINES
**************************************************************************** */
#define	DEF_STDFUNC_1MSEC_BASE	6024

/* ****************************************************************************
	FUNCTIONS
**************************************************************************** */
extern	void stdFuncInit(void);
extern	void wait(int nTime);
extern	int16_t exchg16to10(int16_t unVal);

#endif	// __STDFUNC_H__
