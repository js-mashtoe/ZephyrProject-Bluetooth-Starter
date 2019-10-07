/*
 *  MY BLE Service
 */
#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr.h>
#include <init.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "ble_policy.h"
#include "ble_uuids.h"
#include "ble_service.h"

#define LOG_LEVEL 3 //CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(my_ble);

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
#define CPF_FORMAT_UINT8    4
#define CPF_FORMAT_UTF8     25
#define CPF_FORMAT_OPAQUE   27

static const struct bt_gatt_cpf pattern_cpf = {
  .format      = CPF_FORMAT_UINT8,
  .exponent    = 0,         // no fix-point exponent
  .unit        = 0x2700,    // unitless
  .name_space  = 0x01,      // Bluetoot SIG assigned
  .description = 0x0100,    // Front
};

static const struct bt_gatt_cpf brightness_cpf = {
  .format      = CPF_FORMAT_UINT8,
  .exponent    = 0,         // no fix-point exponent
  .unit        = 0x2700,    // unitless (Q: should be LUX)
  .name_space  = 0x01,      // Bluetoot SIG assigned
  .description = 0x0100,    // Front
};

static const struct bt_gatt_cpf temperature_cpf = {
  .format      = CPF_FORMAT_UINT8,
  .exponent    = 0,         // no fix-point exponent
  .unit        = 0x2700,    // unitless
  .name_space  = 0x01,      // Bluetoot SIG assigned
  .description = 0x0100,    // Front
};

static const struct bt_gatt_cpf command_cpf = {
  .format      = CPF_FORMAT_UINT8,
  .exponent    = 0,         // no fix-point exponent
  .unit        = 0x2700,    // unitless
  .name_space  = 0x01,      // Bluetoot SIG assigned
  .description = 0x0100,    // Front
};

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

static u8_t my_command[] = {
    0x00,
};

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static ssize_t my_read_command(struct bt_conn * conn,
                                 const struct bt_gatt_attr * attr,
                                 void * buf,
                                 u16_t len,
                                 u16_t offset)
{
    const char * value = attr->user_data;

    return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
                             sizeof(my_command));
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static ssize_t my_write_command(struct bt_conn * conn,
                                  const struct bt_gatt_attr *attr,
                                  const void * buf,
                                  u16_t len,
                                  u16_t offset,
                                  u8_t flags)
{
    u8_t * data = attr->user_data;

    //LOG_INF("%s: %02x", __func__, *data);

    if (len != 1) {
        LOG_ERR("%s: INVALID_LENGTH", __func__);
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    ble_enqueue_msg(BLE_EVENT__COMMAND, *(u32_t*)buf);

    if (flags & BT_GATT_WRITE_FLAG_PREPARE) {
        LOG_INF("%s: WRITE_FLAG_PREPARE", __func__);
        return 0;
    }

    if (offset + len > sizeof(my_command)) {
        LOG_ERR("%s: INVALID_OFFSET", __func__);
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    memcpy(data + offset, buf, len);

    return len;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void my_ccc_cfg_changed(const struct bt_gatt_attr * attr, u16_t value)
{
  bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

  LOG_INF("MY notifications %s", notif_enabled ? "enabled" : "disabled");
}

/*---------------------------------------------------------------------------*/
/* MY Service Declaration                                                  */
/*---------------------------------------------------------------------------*/
BT_GATT_SERVICE_DEFINE(my_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_MY_SERVICE),
    BT_GATT_CHARACTERISTIC(BT_UUID_MY_NOTIFY, BT_GATT_CHRC_NOTIFY,
    BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CCC(my_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(BT_UUID_MY_COMMAND,
        (BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_EXT_PROP),
        (BT_GATT_PERM_READ | BT_GATT_PERM_WRITE | BT_GATT_PERM_PREPARE_WRITE),
        my_read_command, my_write_command, &my_command),
    BT_GATT_CUD("Command", BT_GATT_PERM_READ),
    BT_GATT_CPF(&command_cpf),
);

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
int my_notify(u32_t cmd, u32_t code)
{
    struct {
        u32_t cmd;
        u32_t code;
    } data;

    data.cmd = cmd;
    data.code = code;

    if (is_connected()) {
        int rc = bt_gatt_notify(NULL, &my_svc.attrs[1], &data, sizeof(data));
        return rc;
    }
    else {
        return -ENOTCONN;
    }
}

