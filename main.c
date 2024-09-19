/************** ECE2049 DEMO CODE ******************/
/**************  25 August 2021   ******************/
/***************************************************/

#include <msp430.h>

/* Peripherals.c and .h are where the functions that implement
 * the LEDs and keypad, etc are. It is often useful to organize
 * your code by putting like functions together in files.
 * You include the header associated with that file(s)
 * into the main file of your project. */
#include "peripherals.h"
#include "timers.h"
//#define CALADC12_15V_30C *((unsigned int *)0x1A1A)
//#define CALADC12_15V_85C *((unsigned int *)0x1A1C)
#define CALADC12_25V_30C *((unsigned int *)0x1A22)
#define CALADC12_25V_85C *((unsigned int *)0x1A24)
#define MA_PER_BIT 0.244 // =1.0A/4096
// Declare globals here
    unsigned int in_value;
    long unsigned int timerA2_cnt;
    //unsigned int in_temp;
// Main
#pragma vector=TIMER2_A0_VECTOR
__interrupt void TimerA2_ISR (void)
    {
        timerA2_cnt++;
        if(timerA2_cnt == 31536000){
            timerA2_cnt = 0;
        }
    }
void runTimerA2(){
    TA2CTL = TASSEL_1 + MC_1 + ID_0;
    TA2CCR0 = 32767; // 327+1 = 328 ACLK tics = ~1/100 seconds
    TA2CCTL0 = CCIE;
}

void setButtons(){
     P7SEL = P7SEL & ~(BIT4 | BIT0);
     P7DIR = P7DIR & ~(BIT4 | BIT0);
     P3SEL = P3SEL & ~(BIT6);
     P3DIR = P3DIR & ~(BIT6);
     P2SEL = P2SEL & ~(BIT2);
     P2DIR = P2DIR & ~(BIT2);
     P7REN = P7REN | (BIT4 | BIT0);
     P7OUT = P7OUT | (BIT4 | BIT0);
     P3REN = P3REN | BIT6;
     P3OUT = P3OUT | BIT6;
     P2REN = P2REN | BIT2;
     P2OUT = P2OUT | BIT2;
 }

 char buttonPress(){
     char button = 0;
     if(~P7IN & BIT0){
         button = button | BIT0;
     }
     if(~P3IN & BIT6){
         button = button | BIT1;
     }
     if(~P2IN & BIT2){
         button = button | BIT2;
     }
     if(~P7IN & BIT4){
         button = button | BIT3;
     }
     return button;
 }
 void configUserLED(char inbits){
      P6SEL = P6SEL & ~(BIT4 | BIT3 | BIT2 | BIT1);
      P6DIR = P6DIR | (BIT4 | BIT3 | BIT2 | BIT1);
      P6OUT = P6OUT & ~(BIT4 | BIT3 | BIT2 | BIT1);
      char led = 0;
      if(inbits & BIT0){
          led = led | BIT4;
      }
      if(inbits & BIT1){
          led = led | BIT3;
      }
      if(inbits & BIT2){
          led = led | BIT1;
      }
      if(inbits & BIT3){
          led = led | BIT2;
      }
      P6OUT = P6OUT | led;
  }

void main(void)

{
    _BIS_SR(GIE);
    /*UCSCTL4 = 0x0000;
    UCSCTL5 = 0x0000;
    UCSCTL6 = 0x0000;
    UCSCTL7 = 0x0000;
    UCSCTL8 = 0x0000;*/


    unsigned char currKey=0, dispSz = 3;
    unsigned char dispThree[3];

    dispThree[0] = ' ';
    dispThree[2] = ' ';

    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer. Always need to stop this!!
                               // You can then configure it properly, if desired



    // Useful code starts here
    initLeds();

    configDisplay();
    configKeypad();

    unsigned int in_current,in_temp;
    float milliamps, tempC;
    // Reset REFMSTR to hand over control of internal reference
    // voltages to ADC12_A control registers
    REFCTL0 &= ~REFMSTR;
    // Initialize control register ADC12CTL0 = 0000 1001 0111 0000
    // SHT0x=9h (384 clk cycles), MCS=1=burst thru selected chans.,
    // REF2_5V = 1 (2.5V), REFON = 1 = use internal reference volts
    // and ADC12ON = 1 = turn ADC on
    ADC12CTL0=ADC12SHT0_9|ADC12REFON|ADC12REF2_5V|ADC12ON|
    ADC12MSC;
    // Initialize control register ADC12CTL1 = 0000 0010 0000 0010
    // ADC12CSTART ADDx = 0000 = start conversion with ADC12MEM0,
    // ADC12SHSx = 00 = use SW conversion trigger, ADC12SC bits
    // ADC12SHP = 1 = SAMPCON signal sourced from sampling timer,
    // ADC12ISSH = 0 = sample input signal not inverted,
    // ADC12DIVx = 000= divide ADC12CLK by 1,
    // ADC12SSEL=00= ADC clock ADC12OSC (~5 MHz),
    // ADC12CONSEQx = 01 = sequence of channels converted once
    // ADC12BUSY = 0 = no ADC operation active
    ADC12CTL1 = ADC12SHP+ADC12CONSEQ_1;
    // Set conversion memory control registers for the 2 channels
    // ADC12MCTL0: EOS = 0, SREF =001 = voltage refs = GND to Vref+
    // INCHx = 0000
    ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_0;
    // ADC12MCTL1: EOS = 1, SREF =001 = voltage refs = GND to Vref+
    // INCHx = 1010
    ADC12MCTL1 = ADC12SREF_1 + ADC12INCH_10 + ADC12EOS;
    // Set Port 6 Pins 0 to FUNCTION mode (=1) for ADC12
    P6SEL = P6SEL | BIT0;

   long unsigned int test = 31535998;
   long unsigned int currSec = 17708400; //17708400
   timerA2_cnt = currSec;
   int state = 0;
   char disp;
   displayTime(currSec);
   runTimerA2();
   unsigned int scroll = 0;
   int readingCount = 0;
   float avgTemp = 0;
   int i = 0;
   float readings[35] = {0};
   char button = buttonPress();
   while (1)    // Forever loop
    {
       ADC12CTL0 |= ADC12SC + ADC12ENC;
       while (ADC12CTL1 & ADC12BUSY) // poll busy bit
       __no_operation();
       in_current = ADC12MEM0 & 0x0FFF; // keep only low 12 bits
       in_temp = ADC12MEM1 & 0x0FFF; // keep only low 12 bits
       milliamps = (float)in_current * MA_PER_BIT;
       tempC = (float)(((long)in_temp-CALADC12_25V_30C)*(85 - 30))/
       (CALADC12_25V_85C - CALADC12_25V_30C) + 30.0;

       button = buttonPress();

        if(currSec != timerA2_cnt){
            //displayTime(timerA2_cnt);
            currSec = timerA2_cnt;
            readings[readingCount] = tempC;
            readingCount++;
            if(readingCount == 34){
                readingCount = 0;
            }

            if(currSec % 3 == 0){
                avgTemp = 0;
                for(i = 0; i < 35; i++){
                    avgTemp += readings[i];
                }
                avgTemp = avgTemp / 35;
                Graphics_clearDisplay(&g_sContext);
                displayTime(currSec);
                displayTemp(avgTemp);

            }


        configUserLED(BIT0);
        if(button & BIT0){
            button = BIT7;
            configUserLED(BIT2);
            while(state != 10){
               ADC12CTL0 |= ADC12SC + ADC12ENC;
               while (ADC12CTL1 & ADC12BUSY) // poll busy bit
               __no_operation();
               in_current = ADC12MEM0 & 0x0FFF; // keep only low 12 bits
               in_temp = ADC12MEM1 & 0x0FFF; // keep only low 12 bits
               milliamps = (float)in_current * MA_PER_BIT;
               tempC = (float)(((long)in_temp-CALADC12_25V_30C)*(85 - 30))/ (CALADC12_25V_85C - CALADC12_25V_30C) + 30.0;

                if(currSec != timerA2_cnt){
                    if(currSec % 3 == 0){
                        button = buttonPress();
                    }

                    currSec = timerA2_cnt;
                    Graphics_clearDisplay(&g_sContext);
                    displayTime(currSec);
                    switch(state){
                    case 0: //edit month button0
                        if(in_current > 3095){
                            currSec += 2592000;
                            timerA2_cnt = currSec;
                        }
                        if(in_current < 1000){
                            currSec -= 2592000;
                            timerA2_cnt = currSec;
                        }
                        if(button & BIT3){
                           button = BIT7;
                           state = 10;
                        }
                        if(button & BIT1){
                           button = BIT7;
                           state = 1;
                           configUserLED(BIT1);
                        }

                        break;
                    case 1://edit day button1
                        if(in_current > 3095){
                            currSec += 86400;
                            timerA2_cnt = currSec;
                        }
                        if(in_current < 1000){
                            currSec -= 86400;
                            timerA2_cnt = currSec;
                        }
                        if(button & BIT3){
                           button = BIT7;
                           state = 10;
                        }
                        if(button & BIT2){
                           button = BIT7;
                           state = 2;
                           configUserLED(BIT3);
                        }
                        break;
                    case 2://edit hour button2
                        if(in_current > 3095){
                            currSec += 3600;
                            timerA2_cnt = currSec;
                        }
                        if(in_current < 1000){
                            currSec -= 3600;
                            timerA2_cnt = currSec;
                        }
                        if(button & BIT3){
                           button = BIT7;
                           state = 10;
                        }
                        if(button & BIT0){
                           button = BIT7;
                           state = 3;
                           configUserLED(BIT2);
                        }
                        break;
                    case 3://edit min button0
                        if(in_current > 3095){
                            currSec += 60;
                            timerA2_cnt = currSec;
                        }
                        if(in_current < 1000){
                            currSec -= 60;
                            timerA2_cnt = currSec;
                        }
                        if(button & BIT3){
                           button = BIT7;
                           state = 10;
                        }
                        if(button & BIT1){
                           button = BIT7;
                           state = 4;
                           configUserLED(BIT1);
                        }
                        break;
                    case 4://edit sec button1
                    if(in_current > 3095){
                        currSec += 5;
                        timerA2_cnt = currSec;
                    }
                    if(in_current < 1000){
                        currSec -= 5;
                        timerA2_cnt = currSec;
                    }
                    if(button & BIT3){
                       button = BIT7;
                       state = 10;
                    }
                    if(button & BIT2){
                       button = BIT7;
                       state = 0;
                       configUserLED(BIT2);
                    }



                        break;


                    }

                }
            }
            state = 0;
        }

     }


    }  // end while (1)
}

