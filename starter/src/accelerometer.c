/*
 *   accelerometer.c 
 */
#include <device.h>
#include <errno.h>
#include <sys/util.h>
#include <zephyr.h>
#include <drivers/sensor.h>

#include "accelerometer.h"

#define LOG_LEVEL 3
#include <logging/log.h>
LOG_MODULE_REGISTER(accel);

K_SEM_DEFINE(sem, 0, 1);

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void trigger_handler(struct device *dev, struct sensor_trigger *trigger)
{
    ARG_UNUSED(trigger);

    if (sensor_sample_fetch(dev)) {
        LOG_ERR("sensor_sample_fetch failed");
        return;
    }

    k_sem_give(&sem);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void accelerometer_init(void)
{
    struct device * dev = NULL;
    struct sensor_value accel[3];

    LOG_INF(" entry");

    dev = device_get_binding(CONFIG_LIS3DH_NAME);
    if (dev == NULL) {
        LOG_ERR("Could not get %s device", CONFIG_LIS3DH_NAME);
        return;
    }

    struct sensor_trigger trig = {
        .type = SENSOR_TRIG_THRESHOLD,
        .chan = SENSOR_CHAN_ACCEL_XYZ,
    };  

    if (IS_ENABLED(CONFIG_LIS3DH_TRIGGER)) {
        if (sensor_trigger_set(dev, &trig, trigger_handler)) {
             LOG_ERR("Could not set trigger");
            return;
        }
    }    

    while (1) {
        if (IS_ENABLED(CONFIG_LIS3DH_TRIGGER)) {
            k_sem_take(&sem, K_FOREVER);
        } else {
            if (sensor_sample_fetch(dev)) {
                 LOG_ERR("sensor_sample_fetch failed");
            }
        }

        sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel);

         LOG_INF("AX=%10.2f AY=%10.2f AZ=%10.2f (m/s^2)",
                sensor_value_to_double(&accel[0]),
                sensor_value_to_double(&accel[1]),
                sensor_value_to_double(&accel[2]));


        if (!IS_ENABLED(CONFIG_LIS3DH_TRIGGER)) {
            k_sleep(1000);
        }
    }
}
