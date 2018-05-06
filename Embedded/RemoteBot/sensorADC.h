/*
 * sensorADC.h
 *
 *  Created on: May 5, 2018
 *      Author: Alex Tan
 */

#include <msp430.h>

#ifndef SENSORADC_H_
#define SENSORADC_H_

#define CALADC_15V_30C  *((unsigned int *)0x1A1A)                 // Temperature Sensor Calibration-30 C
#define CALADC_15V_85C  *((unsigned int *)0x1A1C)                 // Temperature Sensor Calibration-85 C

volatile float currentTemp;

void initTimerA(void);

void initTempSensor(void);

#endif /* SENSORADC_H_ */
