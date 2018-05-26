/*
 * gpio.c
 *
 *  Created on: May 6, 2018
 *      Author: tanal
 */

#include <gpio.h>

void clearAllGPIO(void){
    P1OUT &= ~(BIT7 | BIT6 | BIT4 | BIT3); // clear the direction ports
    P2OUT &= ~(BIT7 | BIT5);                // clear debug leds
}

void initGPIO(void)
{
    P1SEL0 |= BIT0 | BIT1;      // set p1.0 and p.1 as tx and rx

    P1DIR |= BIT7 | BIT6 | BIT4 | BIT3;     // 7 is front right, 6 is front left, 4 is back right, 3 is back left

    P2DIR |= BIT7 | BIT5; // debug leds
    clearAllGPIO();
}
