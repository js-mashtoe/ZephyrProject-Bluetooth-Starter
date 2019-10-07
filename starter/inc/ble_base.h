// ble_base.h 

#ifndef __BASE__H__
#define __BASE__H__

#include "ble_policy.h"
#include "ble_service.h"
#include "ble_uuids.h" 

bool is_connected(void);
void bas_notify(void);

int  ble_start_advertising(void);
int  ble_stop_advertising(void);
int  ble_disconnect(void);
int  ble_base_init(void);
#endif  // __BASE__H__
