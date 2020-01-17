/* ****************************************************************************
 Name        : serviceCtrl.c
 Description : BLE EVENT CONTROL FUNCTIONS
**************************************************************************** */
#include "sdk_common.h"
#include <string.h>
#include "ble_srv_common.h"
#include "serviceCtrl.h"

/* ****************************************************************************
	STATIC DEFINES
**************************************************************************** */

/* ****************************************************************************
	STATIC FUNCTIONS
**************************************************************************** */
static void on_connect(ble_hs_t * p_hs, ble_evt_t * p_ble_evt);
static void on_disconnect(ble_hs_t * p_hs, ble_evt_t * p_ble_evt);
//static void on_write(ble_hs_t * p_hs, ble_evt_t * p_ble_evt);
static uint32_t hs_data_char_add(ble_hs_t * p_hs, ble_hs_init_t * p_hs_init);

/* ****************************************************************************
	STATIC WORKS
**************************************************************************** */

/* ----------------------------------------------------------------------------
 Name        : void ble_hs_on_ble_evt(ble_hs_t * p_hs, ble_evt_t * p_ble_evt)
 Argument    : p_hs
			   p_ble_evt
 Result      : nil
 Description : BLE EVENT HANDLER
---------------------------------------------------------------------------- */
void ble_hs_on_ble_evt(ble_hs_t * p_hs, ble_evt_t * p_ble_evt)
{
	switch (p_ble_evt->header.evt_id) {
	//BLE接続時に実行される処理
	case BLE_GAP_EVT_CONNECTED:
		on_connect(p_hs, p_ble_evt);
		break;

	//BLE切断時に実行される処理
	case BLE_GAP_EVT_DISCONNECTED:
		on_disconnect(p_hs, p_ble_evt);
		break;

	//セントラル側から送信されるwriteを受信時に実行される処理
	case BLE_GATTS_EVT_WRITE:
//		on_write(p_hs, p_ble_evt);
		break;

	default:
		// No implementation needed.
		break;
	}
}

/* ----------------------------------------------------------------------------
 Name        : uint32_t ble_hs_init(ble_hs_t * p_hs, const ble_hs_init_t * p_hs_init)
 Argument    : p_hs
   			   p_hs_init
 Result      : NRF_SUCCESS	Successfully added a characteristic.
			   NRF_ERROR_INVALID_ADDR	Invalid pointer supplied.
   			   NRF_ERROR_INVALID_PARAM	Invalid parameter(s) supplied, service handle, Vendor Specific UUIDs, lengths, and permissions need to adhere to the constraints.
   			   NRF_ERROR_INVALID_STATE	Invalid state to perform operation, a service context is required.
   			   NRF_ERROR_FORBIDDEN	Forbidden value supplied, certain UUIDs are reserved for the stack.
   			   NRF_ERROR_NO_MEM	Not enough memory to complete operation.
   			   NRF_ERROR_DATA_SIZE	Invalid data size(s) supplied, attribute lengths are restricted by Maximum attribute lengths.
 Description : BLEサービス初期化処理
---------------------------------------------------------------------------- */
uint32_t ble_hs_init(ble_hs_t * p_hs, ble_hs_init_t * p_hs_init)
{
	uint32_t   err_code;
	ble_uuid_t ble_uuid;

	// Initialize service structure
	p_hs->conn_handle = BLE_CONN_HANDLE_INVALID;
//	p_hs->hs_write_handler = p_hs_init->hs_write_handler;

	// Add service
	ble_uuid128_t base_uuid = {hs_UUID_BASE};
	err_code = sd_ble_uuid_vs_add(&base_uuid, &p_hs->uuid_type);
	VERIFY_SUCCESS(err_code);

	ble_uuid.type = p_hs->uuid_type;
	ble_uuid.uuid = hs_UUID_SERVICE;

	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_hs->service_handle);
	VERIFY_SUCCESS(err_code);

	// Add hs characteristic
	err_code = hs_data_char_add(p_hs, p_hs_init);
	VERIFY_SUCCESS(err_code);

	return NRF_SUCCESS;
}

/* ----------------------------------------------------------------------------
 Name        : uint32_t ble_hs_notify(ble_hs_t * p_hs, uint8_t hs_state)
 Argument    : p_hs
   			   p_hs_init
 Result      : NRF_SUCCESS	Successfully added a characteristic.
			   NRF_ERROR_INVALID_ADDR	Invalid pointer supplied.
   			   NRF_ERROR_INVALID_PARAM	Invalid parameter(s) supplied, service handle, Vendor Specific UUIDs, lengths, and permissions need to adhere to the constraints.
   			   NRF_ERROR_INVALID_STATE	Invalid state to perform operation, a service context is required.
   			   NRF_ERROR_FORBIDDEN	Forbidden value supplied, certain UUIDs are reserved for the stack.
   			   NRF_ERROR_NO_MEM	Not enough memory to complete operation.
   			   NRF_ERROR_DATA_SIZE	Invalid data size(s) supplied, attribute lengths are restricted by Maximum attribute lengths.
 Description : Notifyデータ送信関数
---------------------------------------------------------------------------- */
uint32_t ble_hs_send(ble_hs_t * p_hs, uint8_t hs_state)
{
	ble_gatts_hvx_params_t params;
	uint16_t len = sizeof(hs_state);

	memset(&params, 0, sizeof(params));
	params.type = BLE_GATT_HVX_NOTIFICATION;
	params.handle = p_hs->hs_notify_handles.value_handle;
	params.p_data = &hs_state;
	params.p_len = &len;

	return sd_ble_gatts_hvx(p_hs->conn_handle, &params);
}

/* ----------------------------------------------------------------------------
 Name        : void on_connect(ble_hs_t * p_hs, ble_evt_t * p_ble_evt)
 Argument    : p_hs
   			   p_ble_evt
 Result      : nil
 Description : BLE_GAP_EVT_CONNECTED イベントハンドラ
---------------------------------------------------------------------------- */
static void on_connect(ble_hs_t * p_hs, ble_evt_t * p_ble_evt)
{
	p_hs->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

/* ----------------------------------------------------------------------------
 Name        : void on_disconnect(ble_hs_t * p_hs, ble_evt_t * p_ble_evt)
 Argument    : p_hs
			   p_ble_evt
 Result      : nil
 Description : BLE_GAP_EVT_DISCONNECTED イベントハンドラ
---------------------------------------------------------------------------- */
static void on_disconnect(ble_hs_t * p_hs, ble_evt_t * p_ble_evt)
{
	UNUSED_PARAMETER(p_ble_evt);
	p_hs->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/* ----------------------------------------------------------------------------
 Name        : uint32_t hs_data_char_add(ble_hs_t * p_hs, , const ble_hs_init_t * p_hs_init)
 Argument    : p_hs
   			   p_hs_init
 Result      : NRF_SUCCESS	Successfully added a characteristic.
			   NRF_ERROR_INVALID_ADDR	Invalid pointer supplied.
   			   NRF_ERROR_INVALID_PARAM	Invalid parameter(s) supplied, service handle, Vendor Specific UUIDs, lengths, and permissions need to adhere to the constraints.
   			   NRF_ERROR_INVALID_STATE	Invalid state to perform operation, a service context is required.
   			   NRF_ERROR_FORBIDDEN	Forbidden value supplied, certain UUIDs are reserved for the stack.
   			   NRF_ERROR_NO_MEM	Not enough memory to complete operation.
   			   NRF_ERROR_DATA_SIZE	Invalid data size(s) supplied, attribute lengths are restricted by Maximum attribute lengths.
 Description : notify Characteristicを追加する初期化処理
---------------------------------------------------------------------------- */
static uint32_t hs_data_char_add(ble_hs_t * p_hs, ble_hs_init_t * p_hs_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         =  &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_hs->uuid_type;
    ble_uuid.uuid = hs_UUID_HS_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = MAX_NOTIFY_NUM;//sizeof(uint8_t);
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_hs->service_handle,
    &char_md,
    &attr_char_value,
    &p_hs->hs_notify_handles);
}

/* ----------------------------------------------------------------------------
 Name        : uint32_t hs_indicate_char_add(ble_hs_t * p_hs, const ble_hs_init_t * p_hs_init)
 Argument    : p_hs
   			   p_hs_init
 Result      : NRF_SUCCESS	Successfully added a characteristic.
			   NRF_ERROR_INVALID_ADDR	Invalid pointer supplied.
   			   NRF_ERROR_INVALID_PARAM	Invalid parameter(s) supplied, service handle, Vendor Specific UUIDs, lengths, and permissions need to adhere to the constraints.
   			   NRF_ERROR_INVALID_STATE	Invalid state to perform operation, a service context is required.
   			   NRF_ERROR_FORBIDDEN	Forbidden value supplied, certain UUIDs are reserved for the stack.
   			   NRF_ERROR_NO_MEM	Not enough memory to complete operation.
   			   NRF_ERROR_DATA_SIZE	Invalid data size(s) supplied, attribute lengths are restricted by Maximum attribute lengths.
 Description : indicate Characteristicを追加する初期化処理
---------------------------------------------------------------------------- */
/*
static uint32_t hs_indicate_char_add(ble_hs_t * p_hs, const ble_hs_init_t * p_hs_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.indicate = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         =  &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_hs->uuid_type;
    ble_uuid.uuid = hs_UUID_INDICATE_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = MAX_NOTIFY_NUM;//sizeof(uint8_t);
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_hs->service_handle, 
    &char_md,
    &attr_char_value,
    &p_hs->hs_indicate_handles);
}
*/
