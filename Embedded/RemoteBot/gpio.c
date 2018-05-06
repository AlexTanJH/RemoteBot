/*
 * gpio.c
 *
 *  Created on: May 6, 2018
 *      Author: tanal
 */

#include <gpio.h>

void initGPIO(void)
{
    P1SEL0 |= BIT0 | BIT1;      // set p1.0 and p.1 as tx and rx
    P1DIR |= BIT7;              // pin 7 output;
    P1OUT &= ~BIT7;             // initialize pin 7 to low

    P4DIR |= BIT0;
    P4OUT &= ~BIT0;

    P2DIR |= BIT7;
    P2OUT &= ~BIT7;
}
