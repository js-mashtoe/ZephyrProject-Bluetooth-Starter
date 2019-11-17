/*
 *   accel.c 
 */
#include <zephyr.h>
#include <stdio.h>
#include <device.h>
#include <misc/printk.h>

#include "accel.h"

#define MAX_TEST_TIME   10000
#define SLEEPTIME       500

extern uint8_t pbuf[1024];
extern uint8_t * pos;


static struct device * dev;

void accel_get_vector(struct sensor_value axis[3])
{
    if (!dev) {
        printk("dev == NULL\n");
        return;
    }

    if (sensor_sample_fetch(dev) < 0) {
        printk("ACCEL: Sample update error.\n");
        return;
    }

    if (sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, axis) < 0) {
        printk("ACCEL: Cannot read accel channels.\n");
        return;
    }
}

#if !defined(CONFIG_LIS2DH12_TRIGGER)
void print_accel_data(struct device * dev)
{
    struct sensor_value val[3];

    if (sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, val) < 0) {
        printk("ACCEL: Cannot read accel channels.\n");
        return;
    }
}

void test_polling_mode(struct device * dev)
{
    int32_t remaining_test_time = MAX_TEST_TIME;

    printk("ACCEL: test polling beginning\n");

    /* poll the data and print it */
    do {
        if (sensor_sample_fetch(dev) < 0) {
            printk("ACCEL: Sample update error.\n");
            return;
        }

        print_accel_data(dev);

        /* wait a while */
        k_sleep(SLEEPTIME);

        remaining_test_time -= SLEEPTIME;

    } while (remaining_test_time > 0);

    printk("ACCEL: test polling completed\n");
}
#endif // !defined(CONFIG_LIS3DH_TRIGGER)


#ifdef CONFIG_LIS3DH_TRIGGER
static void trigger_handler(struct device * dev,
                            struct sensor_trigger * trigger)
{
    if (trigger->type != SENSOR_TRIG_DELTA &&
        trigger->type != SENSOR_TRIG_DATA_READY) {
        return;
    }

    if (sensor_sample_fetch(dev) < 0) {
        printk("ACCEL: Sample update error.\n");
        return;
    }
}

static void test_anymotion_trigger(struct device * dev)
{
    int32_t remaining_test_time = MAX_TEST_TIME;
    struct sensor_trigger trig;

    /* set up anymotion trigger */

    printk("ACCEL: Anymotion test: shake the device to get anymotion events.\n");

    do {
        /* wait a while */
        k_sleep(SLEEPTIME);
        remaining_test_time -= SLEEPTIME;

    } while (remaining_test_time > 0);

    printk("ACCEL: Anymotion test: finished, removing anymotion trigger...\n");

    trig.type = SENSOR_TRIG_DATA_READY;
    trig.chan = SENSOR_CHAN_ACCEL_XYZ;

    if (sensor_trigger_set(dev, &trig, NULL) < 0) {
        printk("ACCEL: Cannot remove anymotion trigger.\n");
        return;
    }
}

static void test_data_ready_trigger(struct device *lis2dh)
{
    int32_t remaining_test_time = MAX_TEST_TIME;
    struct sensor_trigger trig;

    /* enable data ready trigger */
    trig.type = SENSOR_TRIG_DATA_READY;
    trig.chan = SENSOR_CHAN_ACCEL_XYZ;

    if (sensor_trigger_set(dev, &trig, trigger_handler) < 0) {
        printk("ACCEL: Cannot enable data ready trigger.\n");
        return;
    }

    printk("ACCEL: Data ready test\n");

    do {
        /* wait a while */
        k_sleep(SLEEPTIME);
        remaining_test_time -= SLEEPTIME;

    } while (remaining_test_time > 0);

    printk("ACCEL: Data ready test: finished, removing data ready trigger...\n");

    trig.type = SENSOR_TRIG_DATA_READY;
    trig.chan = SENSOR_CHAN_ACCEL_XYZ;

    if (sensor_trigger_set(dev, &trig, NULL) < 0) {
        printk("ACCEL: Cannot remove data ready trigger.\n");
        return;
    }
}

static void test_trigger_mode(struct device * dev)
{
    printk("ACCEL: %s: enter.\n", __func__);

    test_anymotion_trigger(dev);

    test_data_ready_trigger(dev);

    printk("ACCEL: %s: exit.\n", __func__);
}
#endif /* CONFIG_LIS3DH_TRIGGER */

void accel_init(void)
{
    printk("ACCEL: Binding...\n");

    dev = device_get_binding(CONFIG_LIS3DH_NAME);
    if (!dev) {
        printk("ACCEL: Device not found.\n");
        return;
    }

#if 0 //!defined(CONFIG_LIS3DH_TRIGGER)  // FIXME fix config defines
    printk("Testing the polling mode.\n");
    test_polling_mode(dev);
    printk("ACCEL: Testing the polling mode finished.\n");
#endif

#if 0 //def CONFIG_LIS3DH_TRIGGER
    //#pragma message "building LIS3DH_TRIGGER"
    printk("Testing the trigger mode.\n");
    test_trigger_mode(dev);
    printk("ACCEL: Testing the trigger mode finished.\n");
#else
    (void) test_trigger_mode;
#endif
}

