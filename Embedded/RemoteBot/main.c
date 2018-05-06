#include <msp430.h>
#include <math.h>
#include <stdio.h>


#define CALADC_15V_30C  *((unsigned int *)0x1A1A)                 // Temperature Sensor Calibration-30 C
#define CALADC_15V_85C  *((unsigned int *)0x1A1C)

volatile unsigned int top;
volatile char strStack[10];
volatile float currentTemp;

void sendFloatUART(int f){
    int intFloat = f;
    short isNeg = (f < 0) ? 1 : 0;

//    int intFloat = roundf(f * 100);

    top = 0;

    strStack[top] = '\r';
    top++;
    strStack[top] = '\n';
    top++;

    while (top < 9 ){

        char charToSend;

        if (intFloat == 0){
            strStack[top] = '\0';
            break;
        }

        if(top == 3){
            charToSend = '.';
        }else{
            int currentDigit = intFloat % 10;
            intFloat /= 10;

            charToSend = (char) (currentDigit + 48);
        }

        strStack[top] = charToSend;

        top++;
    }

    strStack[9] = '\0';

    if (isNeg){
        strStack[top] = '-';
        top++;
    }



//    do{
//        while(!(UCA0IFG & UCTXIFG));
//        UCA0TXBUF = strStack[top-1];
//        top--;
//    }while (top > 0);

    UCA0IE |= UCTXIE;
    UCA0IFG |= UCTXIFG;
    UCA0IE &= ~UCRXIE;


}

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

void initTimerA(void){
    TA0CCTL0 |= CCIE;                                               // TACCR0 interrupt enabled
    TA0CCR0 = 65535;                                                // 2^16 / f_refo ~= 0.5 seconds
    TA0CTL = TASSEL__ACLK | MC__UP | TACLR;                         // ACLK, UP mode
}

void initTempsensor(void){
    ADCCTL0 |= ADCSHT_8 | ADCON;                                  // ADC ON,temperature sample period>30us
    ADCCTL1 |= ADCSHP;                                            // use sampling timer
    ADCCTL2 |= ADCRES;                                            // 10-bit conversion results
    ADCMCTL0 |= ADCSREF_1 | ADCINCH_12;                           // ADC channel 12 is the temperature and selecting reference voltage
    ADCIE |=ADCIE0;                                               // Enable the Interrupt request for a completed ADC_B conversion

    // Configure reference
    PMMCTL0_H = PMMPW_H;                                          // Unlock the PMM registers
    PMMCTL2 |= INTREFEN | TSENSOREN;                              // Enable internal reference and temperature sensor
}

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

int main(void)
{
    currentTemp = 0;
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;

    initGPIO();

    configureDCOClock();

    configureBluetoothUART();

    initTimerA();

    initTempsensor();

    FRCTL0 = FRCTLPW | NWAITS_0;  // for debugging

    printf("hello");

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
    case 0x02:  {         // Receive buffer full
        unsigned char recieved = UCA0RXBUF;
//            printf("Received char: %c\n", charData);
//            printf("Received integer: %d\n", recieved);

        switch (recieved)
        {
        case 'a':
            UCA0TXBUF = 'A';
            P4OUT ^= BIT0;
            break;
        case 'b':
            UCA0TXBUF = 'B';
            P1OUT ^= BIT7;
            break;
        case 'c':
            UCA0TXBUF = 'C';
            P2OUT ^= BIT7;
            break;
        case 'd':
            sendFloatUART(currentTemp);
            break;
        default:
            UCA0TXBUF = recieved;
            break;
        }
        //UCA0IE |= UCRXIE;
        __no_operation();
        break;
    }
    case 0x04:           // transmit buffer empty
        if (top > 0){
            UCA0TXBUF = strStack[top-1];
            top--;
        }else {
            UCA0IE &= ~UCTXIE;
            UCA0IE |= UCRXIE;
            UCA0IFG &= ~(0xf);
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
            signed short temp = (ADCMEM0 - CALADC_15V_30C);
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
    ADCCTL0 |= ADCENC | ADCSC;                                    // Sampling and conversion start
}





