#include <msp430.h>
#include <sensorADC.h>
#include <uart.h>
#include <gpio.h>

int main(void)
{
    currentTemp = 0;

    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;

    initGPIO();                 // configuring and setting default values GPIOs

    configureDCOClock();        // configuring mclk, smclk, aclk

    configureBluetoothUART();   // setting uart baud rate and enabling it

    initTimerA();               // setting timer for temperature update

    initTempSensor();           // initializing adc temperature sensor

    // FRCTL0 = FRCTLPW | NWAITS_0;  // for debugging

    __bis_SR_register(LPM3_bits | GIE); // put cpu to standby mode LPM wake up cpu via interrupts and enable interrupts

    return 0;
}

#pragma vector=USCI_A0_VECTOR
__interrupt void bluetoothISR(void)
{
    switch (__even_in_range(UCA0IV, UCTXCPTIFG))
    {
    case 0x00:
        break;
    case 0x02:  {                               // Receive buffer full
        UCA0IE &= ~UCRXIE; // disable reception
        unsigned char recieved = UCA0RXBUF;
//            printf("Received char: %c\n", charData);
//            printf("Received integer: %d\n", recieved);
        P1OUT ^= BIT6;
        switch (recieved)
        {
        case 'a':
            sendChar('A');
            P4OUT ^= BIT0;
            break;
        case 'b':
            sendChar('B');
            P1OUT ^= BIT7;
            break;
        case 'c':
            sendChar('C');
            P2OUT ^= BIT7;
            break;
        case 'd':
            sendFloatUART(currentTemp);
            break;
        case 'x':
            P4OUT |= BIT0;
            break;
        case 'y':
            P4OUT &= ~BIT0;
            break;
        default:
            UCA0IE |= UCRXIE; // disable reception
            break;
        }
        //UCA0IE |= UCRXIE;
        __no_operation();
        break;
    }
    case 0x04:           // transmit buffer empty
        if (top > 0){
            UCA0TXBUF = strStack[top-1];    // transmit char if bottom of stack is not reached
            top--;
        }else {
            UCA0IE &= ~UCTXIE;              // disable transmission if reached bottom of stack
            UCA0IE |= UCRXIE;               // enable reception
            UCA0IFG &= ~(0xf);              // clear all interrupt flags
//            P1OUT &= ~BIT6;
        }
        break;
    case 0x06:
        break;
    case 0x08:          // transmit complete
        break;
    default:
        break;
    }

}

// ADC interrupt service routine
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
{
    switch(__even_in_range(ADCIV,ADCIV_ADCIFG))
    {
        case ADCIV_NONE:
            break;
        case ADCIV_ADCOVIFG:
            break;
        case ADCIV_ADCTOVIFG:
            break;
        case ADCIV_ADCHIIFG:
            break;
        case ADCIV_ADCLOIFG:
            break;
        case ADCIV_ADCINIFG:
            break;
        case ADCIV_ADCIFG: {
            signed short temp = (ADCMEM0 - CALADC_15V_30C); // temperature calculations
            currentTemp = ((long)temp * 10 * (85-30) * 10)/((CALADC_15V_85C-CALADC_15V_30C)*10) + 300;
            break;
        }
        default:
            break;
    }
}

// Timer A0 interrupt service routine
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    ADCCTL0 |= ADCENC | ADCSC;                                    // request temperature update
}





