/* ****************************************************************************
 Name        : main.c
 Description : 熱中症監視システムメインモジュール
**************************************************************************** */
//#define DEF_VALUE_CHECK_DEBUG
//#define DEF_NO_ADERTISING		// debug on

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "boards.h"
#include "app_util_platform.h"

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "fds.h"
#include "bsp_btn_ble.h"
#include "sensorsim.h"
#include "ble_conn_state.h"
#include "nrf_ble_gatt.h"
#include "id_manager.h"
#include "app_timer.h"
#include "nrf_power.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "gpioCtrl.h"
#include "adCtrl.h"
#include "i2cCtrl.h"
#include "sensorCtrl.h"
#include "timerCtrl.h"
#include "stdFunc.h"

/* ****************************************************************************
	DEFINES
**************************************************************************** */
#define	DEF_COMPLETE_LOCAL_NAME		0x09		// アドバタイジングデータTYPE
#define DEVICE_NAME					"SOH-001"	/**< Name of device. Will be included in the advertising data. */
#define APP_BLE_CONN_CFG_TAG		1			/**< A tag identifying the SoftDevice BLE configuration. */

NRF_BLE_GATT_DEF(m_gatt);						/**< GATT module instance. */
BLE_ADVERTISING_DEF(m_advertising);				/**< Advertising module instance. */

#define ADV_INTERVAL_MSEC				100	// アドバタイジングの間隔 (100msec)
#define NON_CONNECTABLE_ADV_INTERVAL	MSEC_TO_UNITS(ADV_INTERVAL_MSEC, UNIT_0_625_MS)	// アドバタイジングの間隔 (100msec)
#define APP_BEACON_INFO_LENGTH			24									// 送信データのうちユーザーが指定できる最大長
#define APP_COMPANY_IDENTIFIER			0x0059								/**< Company identifier for Nordic Semiconductor ASA. as per www.bluetooth.org. */
#define DEAD_BEEF						0xDEADBEEF							// 致命的エラー発生時のメモリ上のシグネチャ値

// DATA STATUS AREA
#define	DEF_STATUS_TEMP_ENABLE	0x0001
#define	DEF_STATUS_HUM_ENABLE	0x0002
#define	DEF_STATUS_ACCEL_ENABLE	0x0004
#define	DEF_STATUS_BUTTON_ON	0x4000
#define	DEF_STATUS_BATTERY_LOW	0x8000

#define	DEF_MODE_IDLE			0		// 休憩中
#define	DEF_MODE_MEASUREMENT	1		// 測定中
#define	DEF_ACCEL_STOP_OFFSET	3		// 加速度計の停止状態を判定する際の誤差範囲指定(単位:%)

// 測定モード定義
#define	DEF_MEAS_TEMP			0
#define	DEF_MEAS_ACCEL_READ		1
#define	DEF_MEAS_ACCEL_SEND		2

// スリープ時間定義
#define	DEF_WAIT_READ_ACCEL_GAP		50		// 加速度センサ読出し間隔 単位:msec
#define	DEF_WAIT_READ_SENSOR_GAP	17800	// 測定中センサ読出し間隔 単位:msec	
											//		全体で30sec
											//		センサ起動時間1sec
											//		センサREAD 50msecx20回+1=1.2sec
											//		アドバタイジング時間 10sec
#define	DEF_WAIT_PAIRING_MODE_GAP	1000	// テストスイッチモード
//#define	DEF_WAIT_READ_SENSOR_GAP	100	// センサ読出し間隔 ワールドエンジニアリング向けテスト用
#define	DEF_WAIT_SEND_DATA_GAP		100		// 送信データ更新間隔 単位:msec
//#define	DEF_WAIT_SEND_DATA_GAP		200		// 送信データ更新間隔 単位:msec
//#define	DEF_WAIT_READY_SENSOR		5000	// センサREADY待ち間隔 単位:msec
#define	DEF_WAIT_READY_SENSOR		1000	// センサREADY待ち間隔 単位:msec 1sec待ち 2019.06.07

// 読出し
#define	DEF_ACCEL_READ_MAX			20
#define	DEF_ACCEL_SEND_CNT_PACKET	3
#define	DEF_ACCEL_SEND_MAX			((DEF_ACCEL_READ_MAX + DEF_ACCEL_SEND_CNT_PACKET - 1) / DEF_ACCEL_SEND_CNT_PACKET)

// ステータスフラグ
#define	DEF_STATUS_AD			0x0001
#define	DEF_STATUS_HUM			0x0002
#define	DEF_STATUS_ACCEL		0x0004
#define	DEF_STATUS_DEVICE_NAME	0x0008

#define	DEF_INVALID_DATA		0x8000

#define	DEF_TIMER_ADVERTISING_TIME	10		// 10sec
#define	DEF_TIMER_ADVERTISING_DATA_EXCHG_TIME	300	// アドバタイジング中のデータ切り替えタイミング 300msec
#define	DEF_ADVERTISING_DATAPACKET_MAX	(DEF_ACCEL_SEND_MAX + 1)

// 加速度計における静止状態誤差定義
#define	DEF_LSM303_THRESHOLD	160	// 上下限値に適用
//#define	DEF_LSM303_THRESHOLD	1000	// 上下限値に適用	debug用

#define FLASH_AREA				(0x2fff0)
#define SERIAL_NO_SIZE			(8)
/* ****************************************************************************
	STATIC VARIABLES
**************************************************************************** */
static ble_gap_adv_params_t m_adv_params; /**< Parameters to be passed to the stack when starting advertising. */
static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */

// YOUR_JOB: Use UUIDs for service(s) used in your application.
//static ble_hs_t _hs;
//static ble_gap_addr_t _macAddress;
int16_t _nAccelData[DEF_ACCEL_READ_MAX + 1][3];	// 最新の加速度データ
int16_t _nAccelBack[DEF_ACCEL_READ_MAX + 1][3];
uint16_t _unAdc[2];								// 最新のサーミスタデータ
uint16_t _unHum;								// 最新の湿度計データ
uint8_t _uManufData[APP_BEACON_INFO_LENGTH];	// 送信用バッファ
uint32_t _unLongSeq;							// シーケンス番号->生存中延々とカウント
uint16_t _unBatteryLow;							// バッテリロー検出
bool _bPairMode;								// ペアリングボタン押下状態
bool _bTemp;									// サーミスタデータ有効フラグ
bool _bHum;										// 湿度計データ有効フラグ
uint8_t _str_serial_number[SERIAL_NO_SIZE];  // Serial number should be 8 bytes "00000000":"FFFFFFFF". It should be ASCII Code.(Not binary), should be placed at the last portion of flash memory                         

/* ****************************************************************************
	STATIC FUNCTIONS
**************************************************************************** */
static bool init(void);
static bool sensorProcess(void);
static void advertisingProcess(void);
static void getAccelData(uint16_t unAccelReadCnt);
static void dataExchg(uint16_t unSendSeqNum, uint16_t unSendPacketNum);
static void dataExchgA(uint16_t unPackectNum, int unLen);
static uint16_t setTemp(uint16_t unSendSeqNum);
void flash_load_device_name(uint8_t* serial_number);
static uint16_t setAccel(uint16_t unSendSeqNum, uint16_t unSendPacketNum);

static void sleep_mode_enter(void);
static void ble_stack_init(void);
static void bsp_event_handler(bsp_event_t event);
static void advertising_init(void);
static void buttons_leds_init(void);
static void log_init(void);
static void powerManage(uint8_t uMode);
void advertising_start(void);
static void measProcess(void);
static bool isActive(void);
static bool isAccelStop(void);
static void funcTest(void);

#ifdef DEF_VALUE_CHECK_DEBUG
static bool checkValue(void);
#endif

static void gap_params_init(void)
{
    ret_code_t              err_code;
//    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                           strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);
/*
    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_THERMOMETER);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
*/
}
/* ----------------------------------------------------------------------------
 Name        : int main(void)
 Argument    : nil
 Result      : nil
 Description : main関数です。
---------------------------------------------------------------------------- */
static bool _bTestMode = false;
int main(void)
{
	init();

	if (_bTestMode) funcTest();			// function test routine.

	ble_stack_init();
	gap_params_init();
	wait(DEF_STDFUNC_1MSEC_BASE * 20);	// SENSOR READY WAIT 90msec
	advertising_init();
	wait(DEF_STDFUNC_1MSEC_BASE * 20);	// 

	timerStartInactive();

	for (;;) {
		sensorProcess();
		advertisingProcess();

		_unLongSeq++;
		if (_unLongSeq>65534) _unLongSeq = 0;	// SEQ-NUMとダブルでデータの同一性検査を行うと仮定
												// このため、SEQ-NUMの最大値 0-15(16種類) で割り切れない値で折り返す。

		isPairingSwOff();							// sleep起床フラグをLOW
		timerSetWakeup(DEF_WAIT_READ_SENSOR_GAP);	// アドバタイジング時間が終われば30秒寝る
		powerManage(DEF_MODE_MEASUREMENT);
	}
}

/* ----------------------------------------------------------------------------
 Name        : void init(void)
 Argument    : nil
 Result      : nil
 Description : 初期化
---------------------------------------------------------------------------- */
static bool init(void)
{
	uint16_t i, j;

	for (i=0; i<DEF_ACCEL_READ_MAX + 1; i++) {
		for (j=0; j < 3; j++) {	// x,y,z
			_nAccelData[i][j] = 0;
			_nAccelBack[i][j] = 0;
		}
	}
	_unAdc[0] = 0;
	_unAdc[1] = 0;
	_unHum = 0;
	memset((char*)_uManufData, 0x00, APP_BEACON_INFO_LENGTH);
	_bPairMode = false;
	_bTemp = false;
	_bHum = false;
	_unLongSeq = 0;
	_unBatteryLow = 0;

	// Initialize.
	log_init();
	timerInit();
	wait(DEF_STDFUNC_1MSEC_BASE * 90);	// SENSOR READY WAIT 90msec
	buttons_leds_init();

	gpioPower(true);						// センサーの電源ON
	wait(DEF_STDFUNC_1MSEC_BASE * 10);		// SENSOR READY WAIT 90msec
	sensorInit();
	wait(DEF_STDFUNC_1MSEC_BASE * 90);	// SENSOR READY WAIT 90msec
	gpioPower(false);						// センサーの電源OFF
	gpioInit();

	return true;
}

/* ----------------------------------------------------------------------------
 Name        : void init(void)
 Argument    : nil
 Result      : nil
 Description : 初期化
---------------------------------------------------------------------------- */
static bool sensorProcess(void)
{
	uint8_t uMode = DEF_MODE_MEASUREMENT;
	/*
	   システムは、通常睡眠状態で、GPIOの変化で起動する。
	   このため、起動した場合、無条件測定モードとなり、規定時間の静止状態で電源OFFとなる。
	   */
	uint16_t unMeasMode = DEF_MEAS_TEMP;
	uint16_t unAccelReadCnt = 0;
	bool bExit = false;
	bool bStop = false;

//gpioDebug(true);	// debug mic

	gpioPower(true);						// センサーの電源ON
	wait(DEF_STDFUNC_1MSEC_BASE * 10);		// SENSOR READY WAIT 10msec 4msec以上あけること
	sensorInit2();							// センサー init
	wait(DEF_STDFUNC_1MSEC_BASE * 50);		// SENSOR READY WAIT 40msec
	readBatteryLow();						// バッテリロー検出
	wait(DEF_STDFUNC_1MSEC_BASE * 40);		// SENSOR READY WAIT 50msec センサ初期化から90msec空ける

	timerSetWakeup(DEF_WAIT_READY_SENSOR);		// power on 後 5sec待つテスト
	powerManage(uMode);		// sleep or shutdown


	timerSetWakeup(DEF_WAIT_READ_ACCEL_GAP);	// 加速度センサ読み込みスパン設定

	while (!bExit) {
		switch (unMeasMode) {
		case DEF_MEAS_TEMP:
			measProcess();
			unAccelReadCnt = 0;
			unMeasMode = DEF_MEAS_ACCEL_READ;
			break;
		case DEF_MEAS_ACCEL_READ:
			sensorRead(EN_SENSOR_ACCEL);
			getAccelData(unAccelReadCnt++);
			if (unAccelReadCnt>=DEF_ACCEL_READ_MAX) {
				unMeasMode = DEF_MEAS_ACCEL_SEND;
			}
			break;
		default:
		case DEF_MEAS_ACCEL_SEND:
			if (isAccelStop()) {
				gpioPower(false);				// センサーの電源OFF
				uMode = DEF_MODE_IDLE;			// Shutdown
#ifdef DEF_VALUE_CHECK_DEBUG
				uMode = DEF_MODE_MEASUREMENT;		// debug mic
				unMeasMode = DEF_MEAS_ACCEL_SEND;	// debug mic
#endif
			}
			bExit = true;
			break;
		}
		powerManage(uMode);		// sleep or shutdown
	}
	gpioPower(false);			// センサーの電源OFF
#ifdef DEF_VALUE_CHECK_DEBUG
	if (!checkValue()) {		// debug
		bStop = true;
	}
#endif
	return bStop;
}

/* ----------------------------------------------------------------------------
 Name        : void advertisingProcess(void)
 Argument    : nil
 Result      : nil
 Description : アドバタイジング処理
---------------------------------------------------------------------------- */
#define	DEF_LED_TIMER	60	// 1カウント50msec	60->3sec
static void advertisingProcess(void)
{
	uint16_t unSendPacketNum = 0;
	uint16_t unSendSeqNum = 0;
	uint16_t unLedTimer = 0;

	// SWITCH START IS LED ON
	if (isWakeupPairingSw()) {
		gpioDebug(true);
		unLedTimer = 0;
	}

	unSendSeqNum = 0;
	advertising_init();

	timerSetWakeup(DEF_WAIT_READ_ACCEL_GAP);	// 加速度センサ読み込みスパン設定
	timerStartAdv(DEF_TIMER_ADVERTISING_TIME);
	timerStartAdvDataExchg(DEF_TIMER_ADVERTISING_DATA_EXCHG_TIME);

	dataExchg(unSendSeqNum, unSendPacketNum++);

#ifndef DEF_NO_ADERTISING
	advertising_start();
#endif
	while (!timerIsAdv()) {
		if (timerIsAdvDataExchg()) {
			timerStartAdvDataExchg(DEF_TIMER_ADVERTISING_DATA_EXCHG_TIME);	// restart timer
			dataExchg(unSendSeqNum, unSendPacketNum++);
			if (unSendPacketNum>=DEF_ADVERTISING_DATAPACKET_MAX) {
				unSendPacketNum = 0;
				unSendSeqNum++;
			}
		}
		powerManage(DEF_MODE_MEASUREMENT);
		unLedTimer++;
		if (unLedTimer > DEF_LED_TIMER) {
			gpioDebug(false);
		}
	}
	sd_ble_gap_adv_stop();				// stop advertising
}

/* ----------------------------------------------------------------------------
 Name        : void getAccelData(uint16_t unSendSeqNum, uint16_t unAccelReadCnt)
 Argument    : uint16_t unSendSeqNum	データ送信回数何回目
   			   uint16_t unAccelReadCnt	パケット番号
 Result      : nil
 Description : _uManufData に送信用データをセットし、送信関数に渡します。
---------------------------------------------------------------------------- */
static void dataExchg(uint16_t unSendSeqNum, uint16_t unSendPacketNum)
{
	uint16_t unLen;

	if (unSendPacketNum==0) {
		unLen = setTemp(unSendSeqNum);
	}
	else {
		unLen = setAccel(unSendSeqNum, unSendPacketNum);
	}
	dataExchgA(unSendPacketNum, unLen);
}

/* ----------------------------------------------------------------------------
 Name        : void dataExchgA(uint16_t unPacketNum, int unLen)
 Argument    : int unLen Munufacture 送信データ長
 Result      : nil
 Description : アドバタイジング送信バッファに_uManufDataを設定します。
---------------------------------------------------------------------------- */
static void dataExchgA(uint16_t unPacketNum, int unLen)
{
	uint32_t err_code;
	ble_advdata_t advdata;
	ble_advdata_manuf_data_t manuf_specific_data;
	uint8_t flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED |  BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

	manuf_specific_data.company_identifier = APP_COMPANY_IDENTIFIER;
	manuf_specific_data.data.p_data = (uint8_t *)_uManufData;
	manuf_specific_data.data.size = unLen;

	// Build and set advertising data.
	memset(&advdata, 0, sizeof(advdata));

	if (unPacketNum==0) {
		advdata.name_type = BLE_ADVDATA_FULL_NAME;
	}
	else {
		advdata.name_type = BLE_ADVDATA_NO_NAME;
	}
	advdata.flags = flags;
	advdata.p_manuf_specific_data = &manuf_specific_data;

	err_code = ble_advdata_set(&advdata, NULL);
	APP_ERROR_CHECK(err_code);

	// Initialize advertising parameters (used when starting advertising).
	memset(&m_adv_params, 0, sizeof(m_adv_params));

	m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
	m_adv_params.p_peer_addr = NULL;    // Undirected advertisement.
	m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
	m_adv_params.interval    = NON_CONNECTABLE_ADV_INTERVAL;
	m_adv_params.timeout     = 0;       // Never time out.
}

/* ----------------------------------------------------------------------------
 Name        : uint16_t setTemp(uint16_t unSendSeqNum)
 Argument    : uint16_t unSendSeqNum	送信回数何回目
 Result      : nil
 Description : 温度データを _uManufData に設定します。
---------------------------------------------------------------------------- */
uint16_t setTemp(uint16_t unSendSeqNum)
{
	uint8_t *p = _uManufData;	// 送信用バッファ
 	uint8_t unLen;
	uint8_t unType;
	uint8_t device_name[SERIAL_NO_SIZE - 1];

	// read sensor-datas
	uint16_t unStatus;

	unStatus  = _bTemp;
	unStatus |= (_bHum<<1);
	unStatus |= DEF_STATUS_DEVICE_NAME;	// add device name
	unStatus |= (isWakeupPairingSw()<<14);
	unStatus |= (isBatteryLow()<<15);
	unStatus |= ((unSendSeqNum & 0x000f)<<4);		// Send Seq-Number

	// read DEVICE_NAME : put serial number here!
	flash_load_device_name(_str_serial_number);
	unLen = 7;                                // Keep current specification
	unType = _str_serial_number[0];           // put Serial Number here!
	device_name[0] = _str_serial_number[1];   // put Serial Number here!
	device_name[1] = _str_serial_number[2];   // put Serial Number here!
	device_name[2] = _str_serial_number[3];   // put Serial Number here!
	device_name[3] = _str_serial_number[4];   // put Serial Number here!
	device_name[4] = _str_serial_number[5];   // put Serial Number here!
	device_name[5] = _str_serial_number[6];   // put Serial Number here!
	device_name[6] = _str_serial_number[7];   // put Serial Number here!

	// data set to advertising buff
	memset(p, 0x00, APP_BEACON_INFO_LENGTH);
	memcpy((char*)p, &unStatus, sizeof(uint16_t));
	p += sizeof(uint16_t);
	memcpy((char*)p, &_unAdc[0], sizeof(uint16_t) * 2);
	p += sizeof(uint16_t) * 2;
	memcpy((char*)p, &_unHum, sizeof(uint16_t) * 1);
	p += sizeof(uint16_t) * 1;
	memcpy((char*)p, &_unLongSeq, sizeof(uint16_t) * 1);
	p += sizeof(uint16_t) * 1;

	// LEN(1) | TYPE(1) | DEVICE_NAME(7)
	memcpy((char*)p, &unLen, 1);
	p += sizeof(uint8_t);
	memcpy((char*)p, &unType, 1);
	p += sizeof(uint8_t);
	memcpy((char*)p, device_name, 7);
	p += 7;
	return (uint32_t)p - (uint32_t)_uManufData;
}

// load serial number from flash
void flash_load_device_name(uint8_t* str_serial_number)
{
	memcpy((uint8_t*)str_serial_number, (uint8_t*)FLASH_AREA, SERIAL_NO_SIZE);
}
/* ----------------------------------------------------------------------------
 Name        : void setAccel(uint16_t unSendSeqNum, uint16_t unSendPacketNum)
 Argument    : uint16_t unSendSeqNum	データ送信回数何回目
   			   uint16_t unAccelReadCnt	パケット番号(1-7)
 Result      : nil
 Description : 加速度データ読出し処理
---------------------------------------------------------------------------- */
#define	DEF_ACCEL_PACKET_LEN	22		// status(2) + dataXYZ(6) x 3(data) + Long sequence(2)
static uint16_t setAccel(uint16_t unSendSeqNum, uint16_t unSendPacketNum)
{
	uint16_t unLen;
	uint16_t unDataCnt;
	uint16_t unStatus = DEF_STATUS_ACCEL_ENABLE | ((unSendPacketNum - 1)<<11);
	uint8_t *p;
	uint16_t unIndex = (unSendPacketNum - 1) * DEF_ACCEL_SEND_CNT_PACKET;

	if (unSendPacketNum < DEF_ACCEL_SEND_MAX) {
		unLen = DEF_ACCEL_PACKET_LEN;
		unDataCnt = DEF_ACCEL_SEND_CNT_PACKET;
	}
	else {
		unLen = DEF_ACCEL_PACKET_LEN - sizeof(int16_t) * 3;		// XYZ
		unDataCnt = DEF_ACCEL_SEND_CNT_PACKET - 1;
	}
	unStatus |= DEF_STATUS_DEVICE_NAME;	// add device name, should be always set to 1.  Getaway checks it.
	unStatus |= (isWakeupPairingSw()<<14);
	unStatus |= (isBatteryLow()<<15);
	unStatus |= ((unSendSeqNum & 0x000f)<<4);		// Send Seq-Number

	p = _uManufData;
	memcpy(p, &unStatus, sizeof(uint16_t));
	p += sizeof(uint16_t);
	memcpy(p, &_nAccelData[unIndex], sizeof(int16_t) * 3 * unDataCnt);
	p += sizeof(int16_t) * 3 * unDataCnt;
	memcpy((char*)p, &_unLongSeq, sizeof(uint16_t) * 1);
	p += sizeof(uint16_t) * 1;

	return unLen;
}

/* ----------------------------------------------------------------------------
 Name        : void getAccelData(uint16_t unAccelReadCnt)
 Argument    : nil
 Result      : nil
 Description : 温度データ読出し処理
---------------------------------------------------------------------------- */
static int16_t _nAccel[3];
static void getAccelData(uint16_t unAccelReadCnt)
{
	/*
	   タイマー割り込みで起床し、加速度センサを読んだ後、本件に通過するように
	   */
	if (sensorGetData(EN_SENSOR_ACCEL, (uint16_t*)_nAccel)) {
		_nAccelData[unAccelReadCnt][0] = _nAccel[0];
		_nAccelData[unAccelReadCnt][1] = _nAccel[1];
		_nAccelData[unAccelReadCnt][2] = _nAccel[2];
	}
	else {
		_nAccelData[unAccelReadCnt][0] = 0;
		_nAccelData[unAccelReadCnt][1] = 0;
		_nAccelData[unAccelReadCnt][2] = 0;
	}
}

/* ----------------------------------------------------------------------------
 Name        : bool isAccelStop(void)
 Argument    : nil
 Result      : true	false
 Description : 最後1つのデータで判定現在の停止状態をモードを算出して、それを戻り値に返します。
---------------------------------------------------------------------------- */
static bool isAccelStop(void)
{
	bool r = false;

	if (!isActive()) {
		if (timerIsInactive()) {
			r = true;				// 完全停止
		}
	}
	else {
		timerStartInactive();		// restart
	}
	return r;
}

/* ----------------------------------------------------------------------------
 Name        : bool isActive(void)
 Argument    : nil
 Result      : true->変化なし	false->変化有り
 Description : 前回の加速度計と現在の加速度計に大きな変化があったどうかを判定し
   			   その結果を戻り値に返します。
---------------------------------------------------------------------------- */
static void getAve(int16_t nData[][3], int16_t nAve[])
{
	uint16_t i, j;
	int16_t nMin[3] = {0x7fff, 0x7fff, 0x7fff};
	int16_t nMax[3] = {0x8001, 0x8001, 0x8001};
	int32_t lAve[3] = {0, 0, 0};

	for (i=0; i< DEF_ACCEL_READ_MAX; i++) {
		for (j=0; j< 3; j++) {
			if (nMin[j] > nData[i][j]) {
				nMin[j] = nData[i][j];
			}
			if (nMax[j] < nData[i][j]) {
				nMax[j] = nData[i][j];
			}
			lAve[j] += nData[i][j];
		}
	}
	for (j=0; j< 3; j++) {
		lAve[j] -= (nMin[j] + nMax[j]);
		lAve[j] /= (DEF_ACCEL_READ_MAX - 2);
		nAve[j] = lAve[j];
	}
}

/* ----------------------------------------------------------------------------
 Name        : bool isActive(void)
 Argument    : nil
 Result      : true->変化なし	false->変化有り
 Description : 前回の加速度計と現在の加速度計に大きな変化があったどうかを判定し
   			   その結果を戻り値に返します。
---------------------------------------------------------------------------- */
static bool isActive(void)
{
	bool r = false;
	uint16_t i, j, a;
	int16_t nBackAve[3] = {0, 0, 0};
	int16_t nNowAve[3] = {0, 0, 0};

	// 前回データの平均値
	getAve(_nAccelBack, nBackAve);
	// 今回データの平均値
	getAve(_nAccelData, nNowAve);

	for (j=0; j< 3; j++) {
		if (nBackAve[j] > nNowAve[j]) {
			a = nBackAve[j] - nNowAve[j];
		}
		else {
			a = nNowAve[j] - nBackAve[j];
		}
		if (a > DEF_LSM303_THRESHOLD) {
			r = true;
			break;
		}
	}
	for (i=0; i< DEF_ACCEL_READ_MAX; i++) {
		for (j=0; j< 3; j++) {
			_nAccelBack[i][j] = _nAccelData[i][j];
		}
	}
	return r;
}

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/* ----------------------------------------------------------------------------
 Name        : void sleep_mode_enter(void)
 Argument    : nil
 Result      : nil
 Description : GPIOボタン起床を仕込んで、SYSTEM-OFFとする。
   			   起床した際にはプログラムの先頭から動作するらしい。
---------------------------------------------------------------------------- */
static void sleep_mode_enter(void)
{
	ret_code_t err_code;

	// Prepare wakeup buttons.
	err_code = bsp_btn_ble_sleep_mode_prepare();
	APP_ERROR_CHECK(err_code);

	// Go to system-off mode (this function will not return; wakeup will cause a reset).
	err_code = sd_power_system_off();
	APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated when button is pressed.
 */
static void bsp_event_handler(bsp_event_t event)
{
	ret_code_t err_code;

	switch (event) {
	case BSP_EVENT_SLEEP:
		sleep_mode_enter();
		break; // BSP_EVENT_SLEEP

	case BSP_EVENT_DISCONNECT:
		err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
		if (err_code != NRF_ERROR_INVALID_STATE) {
		    APP_ERROR_CHECK(err_code);
		}
		break; // BSP_EVENT_DISCONNECT

	case BSP_EVENT_WHITELIST_OFF:
		if (m_conn_handle == BLE_CONN_HANDLE_INVALID) {
		    err_code = ble_advertising_restart_without_whitelist(&m_advertising);
			if (err_code != NRF_ERROR_INVALID_STATE) {
		        APP_ERROR_CHECK(err_code);
		    }
		}
		break; // BSP_EVENT_KEY_0

	default: break;
	}
}

/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
    uint8_t       flags;

	flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED |  BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    ble_advdata_manuf_data_t manuf_specific_data;

    manuf_specific_data.company_identifier = APP_COMPANY_IDENTIFIER;

	memset((char*)_uManufData, 0x00, APP_BEACON_INFO_LENGTH);
	manuf_specific_data.data.p_data = (uint8_t *)_uManufData;
	manuf_specific_data.data.size   = APP_BEACON_INFO_LENGTH;

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

	advdata.name_type             = BLE_ADVDATA_NO_NAME;
	advdata.flags                 = flags;
    advdata.p_manuf_specific_data = &manuf_specific_data;

    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);

    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
    m_adv_params.p_peer_addr = NULL;    // Undirected advertisement.
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = NON_CONNECTABLE_ADV_INTERVAL;
    m_adv_params.timeout     = 0;       // Never time out.
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(void)
{
    ret_code_t err_code;
    bsp_event_t startup_event;

    err_code = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);
}
/**@brief Function for initializing the nrf log module.
 */

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/* ----------------------------------------------------------------------------
 Name        : void powerManage(uint8_t uMode)
 Argument    : uint8_t uMode	モード
 Result      : nil
 Description : スリープ管理
---------------------------------------------------------------------------- */
static void powerManage(uint8_t uMode)
{
    ret_code_t err_code = sd_app_evt_wait();

	if (uMode==DEF_MODE_MEASUREMENT) {
		/* wakeup timer event */
		__WFE();
	}
	else {
//gpioDebug(false);	// debug mic
//		__WFE();
		/* sleep until an interrupt occurs */
		gpioPower(false);			// センサーの電源OFF
		nrf_power_system_off();		// power off sleep.
		while (1);
	}
}


/**@brief Function for starting advertising.
 */
void advertising_start(void)
{
    ret_code_t err_code = sd_ble_gap_adv_start(&m_adv_params, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
}

/* ----------------------------------------------------------------------------
 Name        : void measProcess(void)
 Argument    : nil
 Result      : nil
 Description : サーミスタと湿度計の読み出し処理
---------------------------------------------------------------------------- */
static void measProcess(void)
{
	if ((_bTemp = sensorRead(EN_SENSOR_TEMP))) {
		_bTemp = sensorGetData(EN_SENSOR_TEMP, _unAdc);
	}
	if ((_bHum = sensorRead(EN_SENSOR_HUM))) {
		_bHum = sensorGetData(EN_SENSOR_HUM, &_unHum);
	}
}


/**
 * @}
 */

uint16_t _unTestData;
int16_t abc;

static void funcTest(void)
{
	bool b = true;

	abc = exchg16to10(0x8080);
	abc = exchg16to10(0x7f70);

	if (abc==0) {
		wait(DEF_STDFUNC_1MSEC_BASE * 90);	// Ready wait 90msec
	}
	i2cSensor();

// i2c 加速度センサテスト
	while(b){
		b=sensorProcess();
	}
	init();
	wait(DEF_STDFUNC_1MSEC_BASE * 90);	// Ready wait 90msec

	b = sensorRead(EN_SENSOR_ACCEL);

	i2cSensor();
	while(b) {
          b = sensorRead(EN_SENSOR_ACCEL);
	}

//	advertising_start(true);

	
	
//	i2cInit();
//	bool b = sensorRead(EN_SENSOR_HUM);

	
	
/*
	uint16_t unAdc[2];
	while (1) {
		adRead(unAdc);
		if (isPairingSw()) break;
	}

	while (1) {
		measProcess();
		if (isPairingSw()) break;
	}
*/
}

#ifdef DEF_VALUE_CHECK_DEBUG
bool checkValue(void)
{
	int16_t maxx,maxy,maxz;
	int16_t minx,miny,minz;
	int32_t avex,avey,avez;
	int16_t maxxi,maxyi,maxzi;
	int16_t minxi,minyi,minzi;
	int16_t cntx,cnty,cntz;
	int16_t threshold,s;
	int16_t i;
	bool r = true;
	
	maxx = -32768;
	maxy = -32768;
	maxz = -32768;
	minx = 32767;
	miny = 32767;
	minz = 32767;
	avex = 0;
	avey = 0;
	avez = 0;;
	maxxi = -1;
	maxyi = -1;
	maxzi = -1;
	minxi = -1;
	minyi = -1;
	minzi = -1;
		
	for (i=0; i<20; i++) {
		if (maxx < _nAccelData[i][0]) {
			maxx = _nAccelData[i][0];
			maxxi = i;
		}
		if (maxy < _nAccelData[i][1]) {
			maxy = _nAccelData[i][1];
			maxyi = i;
		}
		if (maxz < _nAccelData[i][2]) {
			maxz = _nAccelData[i][2];
			maxzi = i;
		}
		if (minx > _nAccelData[i][0]) {
			minx = _nAccelData[i][0];
			minxi = i;
		}
		if (miny > _nAccelData[i][1]) {
			miny = _nAccelData[i][1];
			minyi = i;
		}
		if (minz > _nAccelData[i][2]) {
			minz = _nAccelData[i][2];
			minzi = i;
		}
	}

	for (i=0; i<20; i++) {
		if (i!=maxxi && i!=minxi) {
			avex += _nAccelData[i][0];
		}
		if (i!=maxyi && i!=minyi) {
			avey += _nAccelData[i][1];
		}
		if (i!=maxzi && i!=minzi) {
			avez += _nAccelData[i][2];
		}
	}

	cntx = 20;
	cnty = 20;
	cntz = 20;
	if (maxxi>=0) cntx--;
	if (minxi>=0) cntx--;
	if (maxyi>=0) cnty--;
	if (minyi>=0) cnty--;
	if (maxzi>=0) cntz--;
	if (minzi>=0) cntz--;
	avex /= cntx;
	avey /= cnty;
	avez /= cntz;

	threshold = 320;
	for (i=0; i< 20; i++) {
		if (avex > _nAccelData[i][0]) {
			s = avex - _nAccelData[i][0];
		}
		else {
			s = _nAccelData[i][0] - avex;
		}
		if (s>threshold)
                  r = false;

		if (avey > _nAccelData[i][1]) {
			s = avey - _nAccelData[i][1];
		}
		else {
			s = _nAccelData[i][0] - avey;
		}
		if (s>threshold)
                  r = false;

		if (avez > _nAccelData[i][2]) {
			s = avez - _nAccelData[i][2];
		}
		else {
			s = _nAccelData[i][2] - avez;
		}
		if (s>threshold)
                  r = false;
	}
	return r;
}
#endif
