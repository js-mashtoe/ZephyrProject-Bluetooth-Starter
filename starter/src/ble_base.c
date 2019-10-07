/* 
 *  ble_base.c - BLE base support 
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/services/bas.h>

#include "ble_base.h"

static bool connect_state = false;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
struct bt_conn * default_conn;

static const struct bt_data advert[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL,STARTER_UUID_SERVICE,STARTER_UUID_BASE)
};

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void connected_cb(struct bt_conn * conn, u8_t err)
{
    if (err) {
        printk("Connection failed: err %u\n", err);
    }
    else {
        default_conn = bt_conn_ref(conn);

        printk("Connected\n");

        connect_state = true;
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void disconnected_cb(struct bt_conn * conn, u8_t reason)
{
    printk("Disconnected: reason %u\n", reason);
    u8_t payload = 11;

    if (default_conn) {
        bt_conn_unref(default_conn);
        default_conn = NULL;
        
        ble_enqueue_msg(BLE_EVENT__COMMAND,payload);
    }
    connect_state = false;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

static struct bt_conn_cb conn_callbacks = {
    .connected = connected_cb,
    .disconnected = disconnected_cb,
};

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
bool is_connected(void)
{
    return connect_state;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
int ble_disconnect(void)
{
    printk("%s\n", __func__);

    if (default_conn) {
        struct bt_conn * conn = default_conn;

        bt_conn_unref(default_conn);
        
        default_conn = NULL;
        
        k_sleep(50);  // wait for notifications to complete
        
        bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    }
    return 0;
}

/*---------------------------------------------------------------------------*/
/*  Callback                                                                 */
/*---------------------------------------------------------------------------*/
static void bt_ready(int err)
{
    if (err) {
        printk("Bluetooth initialization failed: %d\n", err);
        return;
    }

    printk("Bluetooth initialized OK\n");
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
int ble_start_advertising(void)
{
    int err;

    err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, advert,
                          ARRAY_SIZE(advert), NULL, 0);
    if (err) {
        printk("Start advertising failed: %d\n", err);
        return err;
    }

    printk("Start advertising OK\n");
    return 0;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
int ble_stop_advertising(void)
{
    int err;

    err = bt_le_adv_stop();
    if (err) {
        printk("Stop advertising failed: %d\n", err);
        return err;
    }

    printk("Stop advertising OK\n");
    return 0;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void auth_cancel(struct bt_conn *conn)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    printk("Pairing cancelled: %s\n", addr);
}

static struct bt_conn_auth_cb  auth_cb_display = {
    .cancel = auth_cancel,
};

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void bas_notify(void)
{
    u8_t battery_level = bt_gatt_bas_get_battery_level();

    battery_level--;

    if (!battery_level) {
        battery_level = 100U;
    }

    bt_gatt_bas_set_battery_level(battery_level);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
int ble_base_init(void)
{
    int err;

    err = bt_enable(bt_ready);
    if (err) {
        printk("Bluetooth initialization failed: %d\n", err);
        return err;
    }

    bt_conn_cb_register(&conn_callbacks);
    bt_conn_auth_cb_register(&auth_cb_display);

    return 0;
}
