/*
 *  accel.h 
 */
#ifndef __ACCEL_H__
#define __ACCEL_H__

#include <stdio.h>
#include <drivers/sensor.h>

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void accel_init(void);
void accel_get_vector(struct sensor_value axis[3]);

#endif  /*__ACCEL_H__ */
