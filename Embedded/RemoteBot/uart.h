#include <msp430.h>

#ifndef UART_H_
#define UART_H_

volatile unsigned int top;
volatile char strStack[10];

void configureDCOClock(void);
void configureBluetoothUART(void);
void sendFloatUART(int);
void sendChar(char);

#endif /* UART_H_ */
