/* ****************************************************************************
 Name        : serviceCtrl.h
 Description : BLE EVENT CONTROL FILE HEADER
**************************************************************************** */
#ifndef _SERVICE_CTRL_H__
#define _SERVICE_CTRL_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ****************************************************************************
	DEFINES
**************************************************************************** */
//サービスUUID。ベースとなるhs_UUID_BASEの0x00, 0x00の4バイトに個別のサービスUUID番号が入る。
//例えばNotifyのUUIDは、112233445566778899AABBCCAAAADDEE　になる。
// 952f37c5-d86b-4e3d-ba1a-60cc493ab4f2 
// https://www.uuidgenerator.net/
#define hs_UUID_BASE		{0x95, 0x2f, 0x37, 0xc5, 0xd8, 0x6d, 0x4e, 0x3d, 0xba, 0x1a, 0x60, 0xcc, 0x49, 0x3a, 0xb4, 0xf2}
#define hs_UUID_SERVICE		0xFFFF
#define hs_UUID_HS_CHAR		0x0510

//最大送信バイト数
#define MAX_NOTIFY_NUM		20
#define MAX_WRITE_NUM		20

/* ****************************************************************************
	ENUMS & STRUCT
**************************************************************************** */
typedef enum {
	BLE_hs_EVT_NOTIFICATION_ENABLED,
	BLE_hs_EVT_NOTIFICATION_DISABLED
} ble_hs_evt_type_t;

typedef struct {
	ble_hs_evt_type_t evt_type;
} ble_hs_evt_t;

// Forward declaration of the ble_hs_t type.
//typedef struct ble_hs_s ble_hs_t;

//typedef void (*ble_hs_write_handler_t) (void * p_hs, uint8_t *receive_buff, uint8_t length);

typedef struct {
	uint16_t unTmp;
} ble_hs_init_t;

typedef struct ble_hs_s {
	uint16_t					service_handle;
	ble_gatts_char_handles_t	hs_write_handles;
	ble_gatts_char_handles_t	hs_notify_handles;
	ble_gatts_char_handles_t	hs_indicate_handles;
	uint8_t						uuid_type;
	uint16_t					conn_handle;
//    ble_hs_write_handler_t    hs_write_handler;
} ble_hs_t;

/* 送信データ構造体 */
typedef struct hs_data_s {
	uint16_t unStatus;			// ステータス
	uint16_t unTemp1;			// 温度1
	uint16_t unTemp2;			// 温度2
	uint16_t unAccX;			// 加速度X
	uint16_t unAccY;			// 加速度Y
	uint16_t unAccZ;			// 加速度Z
	uint16_t unHum;				// 湿度
	uint16_t unNouse[3];		// 未使用
} hs_data_t;

/* ****************************************************************************
	EXTERNAL FUNCTIONS
**************************************************************************** */
extern	uint32_t ble_hs_init(ble_hs_t * p_hs, ble_hs_init_t * p_hs_init);
extern	void ble_hs_on_ble_evt(ble_hs_t * p_hs, ble_evt_t * p_ble_evt);
extern	uint32_t ble_hs_send(ble_hs_t * p_hs, uint8_t hs_state);
//extern	uint32_t ble_hs_notify(ble_hs_t * p_hs, uint8_t hs_state);
//extern	uint32_t notify_cmd(ble_hs_t * p_hs, uint8_t cmd, uint8_t *buff, uint16_t data_len);

#ifdef __cplusplus
}
#endif

#endif // _SERVICE_CTRL_H__

/** @} */