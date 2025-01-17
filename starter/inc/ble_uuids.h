/*
 *  app ble_uuids.h
 */
#ifndef __BLE_UUIDS_H__
#define __BLE_UUIDS_H__

/*
 *  The UUID was generated by the uuidgen utility, which gave
 *      aa9fa4b1-a1ee-457a-af5b-59a82163d898
 *      and hand reversed ...
 *      XYZ_UUID_BASE 0x63,0x21,0xA8,0x59,0x5B,0xAF,0x7A,0x45,0xEE,0xA1,0xB1,0xA4,0x9F,0xAA
 *
 *  The last two hex digits (0xd8, 0x98) were set to 0x00,0x00.
 *      aa9fa4b1-a1ee-457a-af5b-59a821630000
 *  This string was reversed to form a byte array to conform to 
 *  Zephyr's BLE API specifications.
 *      00006321a859-5baf-7a45-eea1-b1a49faa
 *
 *  UUID Base (common part) are the last 14 bytes of UUID.
 *  The first two bytes will be Service or Characteristic unique values.
 *  This is similar to the UUID16-type UUIDs specified by the BT SIG.
 */
#define STARTER_UUID_BASE 0x50,0x6B,0x75,0xCE,0xDD,0x9C,0x56,0x4B,0x87,0x70,0x32,0x10,0x60,0xCE 

#define STARTER_UUID_SERVICE            0x00,0x00
#define STARTER_UUID_NOTIFY             0x01,0x00
#define STARTER_UUID_COMMAND            0x02,0x00

/*
 *  Service UUID: aa9fa4b1-a1ee-457a-af5b-59a821630000
 */
#define BT_UUID_STARTER_SERVICE \
    BT_UUID_DECLARE_128(STARTER_UUID_SERVICE,STARTER_UUID_BASE)

/*
 *   Characteristics UUIDs: aa9fa4b1-a1ee-457a-af5b-59a82163xxxx
 */       

#define BT_UUID_STARTER_NOTIFY   \
    BT_UUID_DECLARE_128(STARTER_UUID_NOTIFY,STARTER_UUID_BASE)

#define BT_UUID_STARTER_COMMAND   \
    BT_UUID_DECLARE_128(STARTER_UUID_COMMAND,STARTER_UUID_BASE)

#endif  // __BLE_UUIDS_H__
