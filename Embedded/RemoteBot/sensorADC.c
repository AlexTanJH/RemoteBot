/*
 * sensorADC.c
 *
 *  Created on: May 6, 2018
 *      Author: tanal
 */

#include <sensorADC.h>

void initTimerA(void){
    TA0CCTL0 |= CCIE;                                               // TACCR0 interrupt enabled
    TA0CCR0 = 16384-1;                                              // 2^14 / f_aclk ~= 0.5 seconds
    TA0CTL = TASSEL__ACLK | MC__UP | TACLR;                         // ACLK, UP mode
}

void initTempSensor(void){
    ADCCTL0 |= ADCSHT_8 | ADCON;                                  // ADC ON,temperature sample period>30us
    ADCCTL1 |= ADCSHP;                                            // use sampling timer
    ADCCTL2 |= ADCRES;                                            // 10-bit conversion results
    ADCMCTL0 |= ADCSREF_1 | ADCINCH_12;                           // ADC channel 12 is the temperature and selecting reference voltage
    ADCIE |=ADCIE0;                                               // Enable the Interrupt request for a completed ADC_B conversion

    // Configure reference
    PMMCTL0_H = PMMPW_H;                                          // Unlock the PMM registers
    PMMCTL2 |= INTREFEN | TSENSOREN;                              // Enable internal reference and temperature sensor
}
