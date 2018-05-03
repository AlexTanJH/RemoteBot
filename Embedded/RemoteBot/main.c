#include <msp430fr4133.h>

void initGPIO(void)
{
    P1SEL0 |= BIT0 + BIT1; // set p1.0 and p.1 as tx and rx
    P1DIR |= BIT7; // pin 7 output;
    P1OUT &= ~BIT7; // initialize pin 7 to low
}

void configureClock(void)
{
    // from MSP430fr4xx User guide 3.2.11.1
    // setting up DCO
    __bis_SR_register(SCG0); // Disable FLL
    CSCTL3 |= SELA__REFOCLK;    // select REFO for FLL
    CSCTL0 = 0; // clear DCO registers
    CSCTL1 &= ~0xE; // clear bits for DCO clock selection 1-3
    CSCTL1 |= DCORSEL_3; // select 8 MHz
    CSCTL2 = FLLD_0 + 243; // no divider and set up multiplier
    __delay_cycles(3); // 3 NOP for stuff to setup
    __bic_SR_register(SCG0); // enable FLL
    while ((CSCTL7 & FLLUNLOCK0 || CSCTL7 & FLLUNLOCK1)); // wait until FLL is locked

    CSCTL4 |= SELA__REFOCLK; // set REFO as aclk source
    CSCTL4 |= SELMS__DCOCLKDIV; // set DCOCLKDIV as MCLK/SMCLK source
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;

    return 0;
}
