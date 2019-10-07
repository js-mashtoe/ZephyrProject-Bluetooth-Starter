/* 
 *  main.c - Application main entry point 
 */
#include <zephyr.h>
#include <misc/printk.h>

#include "ble_policy.h"

#define STACKSIZE 1024
#define PRIORITY 7
#define DELAY_TIME   K_MSEC(1000)

/*---------------------------------------------------------------------------*/
/*  This kicks-off the BLE thread                                            */
/*---------------------------------------------------------------------------*/
void bluetooth_thread(void * id, void * unused1, void * unused2)
{
    printk("%s\n", __func__);

    ble_policy_init();
}

K_THREAD_DEFINE(bluetooth_id, STACKSIZE, bluetooth_thread, NULL, NULL, NULL,
                PRIORITY, 0, K_NO_WAIT);
