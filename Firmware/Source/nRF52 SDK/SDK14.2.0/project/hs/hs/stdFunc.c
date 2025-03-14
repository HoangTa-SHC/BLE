/* ****************************************************************************
 Name        : stdFunc.c
 Description : STANDARD FUNCTIONS
**************************************************************************** */
#include <stdio.h>
#include "boards.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "stdFunc.h"


/* ****************************************************************************
	STATIC DEFINES
**************************************************************************** */

/* ****************************************************************************
	STATIC FUNCTIONS
**************************************************************************** */

/* ****************************************************************************
	STATIC WORKS
**************************************************************************** */

/* ----------------------------------------------------------------------------
 Name        : void stdFuncInit(void)
 Argument    : nil
 Result      : nil
 Description : 初期化処理
---------------------------------------------------------------------------- */
void stdFuncInit(void)
{
}

/* ----------------------------------------------------------------------------
 Name        : void sleep(int nTime)
 Argument    : int nTime	6024/1msec
 Result      : nil
 Description : nTime --> 6024 で1msec
---------------------------------------------------------------------------- */
void wait(int nTime)
{
	int i;
	
	for (i=0; i< nTime; i++) {
		asm("NOP");
	}
}

/* ----------------------------------------------------------------------------
 Name        : int16_t exchg16to10(uint16_t unVal)
 Argument    : uint16_t unVal	処理対象値
 Result      : 10bit データ
 Description : 指定の値を左詰め10ビットデータの二の補数値として、16ビットデータに
   				変換後それを戻り値に返します。
---------------------------------------------------------------------------- */
int16_t exchg16to10(int16_t unVal)
{
	int16_t nRVal;
	
	nRVal = unVal>>6;	// 2の補数をとる
        nRVal *= (-1);

	return nRVal;
}
