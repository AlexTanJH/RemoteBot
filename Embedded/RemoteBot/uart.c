/*
 * uart.c
 *
 *  Created on: May 6, 2018
 *      Author: tanal
 */

#include <uart.h>

// set aclk = refo = 32768Hz, smclk = mclk = 8 MHz
void configureDCOClock(void)
{
    // from MSP430fr4xx User guide 3.2.11.1
    // setting up DCO
    __bis_SR_register(SCG0);        // Disable FLL
    CSCTL3 |= SELREF__REFOCLK;        // select REFO for FLL

    CSCTL0 = 0;                     // clear DCO registers

    CSCTL1 &= ~0xE;                 // clear bits for DCO clock selection 1-3
    CSCTL1 |= DCORSEL_3;            // select 8 MHz

    CSCTL2 = FLLD_0 + 243;          // no divider and set up multiplier

    __delay_cycles(3);              // 3 NOP for stuff to setup

    __bic_SR_register(SCG0);        // enable FLL

    while ((CSCTL7 & FLLUNLOCK0) || (CSCTL7 & FLLUNLOCK1))
        ; // wait until FLL is locked

    CSCTL4 = SELMS__DCOCLKDIV + SELA__REFOCLK; // set REFO as aclk source and set DCOCLKDIV as MCLK/SMCLK source
}

void configureBluetoothUART(void)
{
    UCA0CTLW0 |= UCSWRST;                   // stop eUSCI_A

    UCA0CTLW0 |= UCSSEL__SMCLK;             // set SMCLK as clock for UART

    UCA0MCTLW = UCOS16 | UCBRF_1 | 0x4900; // 9600 baud rate from table 21-5 of user guide
    UCA0BR0 = 52;                // 9600 baud rate from table 21-5 of user guide
    UCA0BR1 = 0x00;

    UCA0CTLW0 &= ~UCSWRST;                  // enable eUSCI_A

    // UCA0IFG &= ~0xF;                        // clear bit0-3 of the interrupt flag registers

    UCA0IE |= UCRXIE;                       // enable receive
    // UCA0IE |= UCTXIE;
}

// UART driver for transmitting multiple bytes of data
void sendFloatUART(int f){              // takes a 3 digit number and sends the number / 10 to the transmit buffer
    int intFloat = f;
    short isNeg = (f < 0) ? 1 : 0;      // check for negative

//    int intFloat = roundf(f * 100);

    top = 0;

    strStack[top] = '\r';       // push clrf to bottom of stack
    top++;
    strStack[top] = '\n';
    top++;

    while (top < 9 ){   // upper limit of 9 characters

        char charToSend;

        if (intFloat == 0){
            strStack[top] = '\0';
            break;                  // add null to top of stack when finished
        }

        if(top == 3){
            charToSend = '.';       // 3 character is the decimal point
        }else{
            int currentDigit = intFloat % 10;
            intFloat /= 10;                     // get current digit and move to next digit

            charToSend = (char) (currentDigit + 48);
        }

        strStack[top] = charToSend;

        top++;
    }

    strStack[9] = '\0';

    if (isNeg){
        strStack[top] = '-';    // add negative sign if needed
        top++;
    }

//    do{
//        while(!(UCA0IFG & UCTXIFG));
//        UCA0TXBUF = strStack[top-1];
//        top--;
//    }while (top > 0);


    UCA0IE |= UCTXIE;       //  enable transmission and request to trasmit character
    UCA0IFG |= UCTXIFG;

}

void sendChar(char c){      // appends clrf to the transmit buffer
    top = 0;

    strStack[top] = '\r';
    top++;
    strStack[top] = '\n';
    top++;

    strStack[top] = c;
    top++;



    UCA0IE |= UCTXIE;       //  enable transmission and request to trasmit character
    UCA0IFG |= UCTXIFG;

}
