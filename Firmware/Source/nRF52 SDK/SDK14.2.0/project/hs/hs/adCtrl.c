/* ****************************************************************************
 Name        : adCtrl.c
 Description : GPIO CONTROL FUNCTIONS
**************************************************************************** */
#include "boards.h"
#include "nrf_saadc.h"
#include "adCtrl.h"

/* ****************************************************************************
	STATIC DEFINES
**************************************************************************** */

// sample
//	https://gitlab.fai.utb.cz/jurenat/Espruino/commit/a178f43e809b07700c4dd95c05215d9c76f4050e
// https://gitlab.fai.utb.cz/jurenat/Espruino/blob/c4e0138434b6384855eaaf10bdd43c00b0c0b610/targets/nrf5x/jshardware.c

/* ****************************************************************************
	STATIC FUNCTIONS
**************************************************************************** */
static uint16_t adReadMain(uint8_t uCH, nrf_saadc_input_t enPin);
static nrf_saadc_value_t nrf_analog_read(void);

/* ****************************************************************************
	STATIC WORKS
**************************************************************************** */
static uint16_t _unAdc[2];
static bool _bEnabled;

/* ----------------------------------------------------------------------------
 Name        : void adInit(void)
 Argument    : nil
 Result      : nil
 Description : 初期化
---------------------------------------------------------------------------- */
void adInit(void)
{
	_unAdc[0] = 0;
	_unAdc[1] = 0;
	_bEnabled = false;
}

/* ----------------------------------------------------------------------------
 Name        : bool adGetData(uint16_t punAdc[])
 Argument    : uint16_t punAdc[]	データ格納バッファ
 Result      : true	false
 Description : A/D値引き渡し
---------------------------------------------------------------------------- */
bool adGetData(uint16_t punAdc[])
{
	if (_bEnabled) {
		punAdc[0] = _unAdc[0];
		punAdc[1] = _unAdc[1];
		_bEnabled = false;
		return true;
	}
	return false;
}

/* ----------------------------------------------------------------------------
 Name        : void adRead(void)
 Argument    : nil
 Result      : nil
 Description : A/D値読出し
---------------------------------------------------------------------------- */
void adRead(void)
{
	_unAdc[0] = adReadMain(DEF_ADC_CH0, DEF_ADC_PIN0);
	_unAdc[1] = adReadMain(DEF_ADC_CH1, DEF_ADC_PIN1);
	_bEnabled = true;
}

/* ----------------------------------------------------------------------------
 Name        : uint16_t adReadMain(uint8_t uCH, uint8_t uPin)
 Argument    :	uCH		チャネル
   				uPin	ポート番号
 Result      : 読み出したAD値
 Description : A/D読出し
---------------------------------------------------------------------------- */
static uint16_t adReadMain(uint8_t uCH, nrf_saadc_input_t enPin)
{
	nrf_saadc_channel_config_t config;

	config.acq_time = NRF_SAADC_ACQTIME_10US;	// 
	config.gain = NRF_SAADC_GAIN1_3;			// 1/3 of input volts
												/* 内部リファレンス+-0.6V使用
	   												測定可能電圧範囲 0.6/Gain(1/3) = +-1.80
	   												-1.8V->0x0000～+1.8V->0x03FF
												*/
	config.mode = NRF_SAADC_MODE_SINGLE_ENDED;
	config.pin_p = enPin;
	config.pin_n = enPin;
	config.reference = NRF_SAADC_REFERENCE_INTERNAL; // INTERNAL as reference.
	config.resistor_p = NRF_SAADC_RESISTOR_DISABLED;
	config.resistor_n = NRF_SAADC_RESISTOR_DISABLED;

	nrf_saadc_enable();
	nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_10BIT);
	nrf_saadc_channel_init(uCH, &config);		// setup config

  return (uint16_t)nrf_analog_read();
}

/* ----------------------------------------------------------------------------
 Name        : uint16_t adReadMain(uint8_t uCH, uint8_t uPin)
 Argument    :	uCH		チャネル
   				uPin	ポート番号
 Result      : 読み出したAD値
 Description : A/D読出し
---------------------------------------------------------------------------- */
static nrf_saadc_value_t nrf_analog_read(void)
{
	nrf_saadc_value_t result;
	nrf_saadc_buffer_init(&result, 1);

	nrf_saadc_task_trigger(NRF_SAADC_TASK_START);

	while(!nrf_saadc_event_check(NRF_SAADC_EVENT_STARTED));
	nrf_saadc_event_clear(NRF_SAADC_EVENT_STARTED);

	nrf_saadc_task_trigger(NRF_SAADC_TASK_SAMPLE);

	while(!nrf_saadc_event_check(NRF_SAADC_EVENT_END));
	nrf_saadc_event_clear(NRF_SAADC_EVENT_END);

	nrf_saadc_task_trigger(NRF_SAADC_TASK_STOP);
	while(!nrf_saadc_event_check(NRF_SAADC_EVENT_STOPPED));
	nrf_saadc_event_clear(NRF_SAADC_EVENT_STOPPED);

	return result;
}
