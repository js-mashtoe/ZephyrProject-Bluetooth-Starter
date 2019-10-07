/*
 *  ble_policy.h
 */
#ifndef __BLE_POLICY_H__
#define __BLE_POLICY_H__

#include "ble_base.h"

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

typedef enum {
	BLE_EVENT__INVALID = 0,
	BLE_EVENT__COMMAND,
	BLE_EVENT__LAST
} ble_event_t;

typedef enum {
	BLE_CMD__INVALID = 0,
	BLE_CMD__DISCONNECT,
	BLE_CMD__CMD1,
	BLE_CMD__CMD2,  
	BLE_CMD__LAST
} ble_cmd_t;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
int  ble_enqueue_msg(ble_event_t charact, u32_t data);
void ble_operation_complete(ble_event_t charact, u32_t code);
void ble_policy_events(void);
int  ble_policy_init(void);
                   
#endif  // __BLE_POLICY_H__