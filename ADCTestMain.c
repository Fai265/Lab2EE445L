// ADCTestMain.c (Lab 2)
// Faisal Mahmood and Coooper Carnahan
// Created: September 12, 2017
// This program samples the ADC 1000 times and then plots
// a pmf graph of the resulting data returned, while also
// calculating the time jitter of the sampling
// TA: Saadallah Kassir
// Last revision: September 22, 2017

// hardware connections
// **********ST7735 TFT and SDC*******************
// ST7735
// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

// ADCSeq3 connected to PE4 in middle of two 2000 Ohm resistor voltage divider

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V 
#include <stdint.h>
#include "ADCSWTrigger.h"
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "BufferFunctions.h"
#include "ST7735.h"
#include "Switches and LEDs.h"
#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))



void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

volatile uint32_t ADCvalue;      // stores ADC value just returned
static uint32_t startTime = 0;   // stores initial time and updates after every calc of difference from current 
static uint32_t currentTime = 0; 
static uint8_t dataFlag = 0;     //used to tell whether program is ready to plot PMF
static int index = 0;
static int ADCTime[1000];
static int ADCData[1000];
static int jitter=0;
static int pmf_array[4096]={0};

// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void){
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD; // configure for periodic mode
  TIMER0_TAILR_R = 80000;     // start value for 1000 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R |= 1<<19;              // enable interrupt 19 in NVIC
}

void Timer2A_Init(void){ //This is for part D of the lab manual 
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x04;      // activate timer
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER2_CTL_R &= ~TIMER_CTL_TAEN; // disable timer2A during setup
  TIMER2_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer2 initialization ****
  TIMER2_TAMR_R = TIMER_TAMR_TAMR_PERIOD; // configure for periodic mode
  TIMER2_TAILR_R =79800;     // start value for ~1000 Hz interrupts
  TIMER2_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;// clear timer2 timeout flag
  TIMER2_CTL_R |= TIMER_CTL_TAEN;  // enable timer2 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer2A=priority 1
  NVIC_PRI5_R = (NVIC_PRI4_R&0x00FFFFFF)|0x20000000; // top 3 bits
  NVIC_EN0_R |= 1<<23;              // enable interrupt 19 in NVIC
}

void Timer1_Init(void){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
	int delay = SYSCTL_RCGCTIMER_R;	// allow time for the Timer to activate
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = 0xFFFFFFFF;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 4
  // interrupts enabled in the main program after all devices initialized
  // vector number 37, interrupt number 21
  NVIC_EN0_R |= 1<<21;           // 9) enable IRQ 21 in NVIC	
	TIMER1_CTL_R = 0x00000001;
	startTime = TIMER1_TAR_R;
  return;
}

void Timer0A_Handler(void){
	PF2 ^= 0x04;
	PF2 ^= 0x04;  												//toggles to know when the ISR has started
  currentTime = TIMER1_TAR_R;
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  PF2 ^= 0x04;													// used to test the time ADC conversion takes. Can help to turn off first two toggles
	ADCvalue = ADC0_InSeq3();	
	PF2 ^= 0x04;													// used to test the time ADC conversion takes...
	if(index < 1000){
	  ADCData[index] = ADCvalue;
		ADCTime[index] = (startTime - currentTime);
		startTime=currentTime;							// sets new startTime for the ADCTime values to be easier to read
		index++;
	}
	else{
		DisableInterrupts();								// turns off interrupts to avoid slowing things down
																				// by interrupting after arrays are full
		index = 0;
		dataFlag = 1;												// tells main that the calculations are complete
	}
}

int main(void){ 
  PLL_Init(Bus80MHz);                   // 80 MHz
	ST7735_InitR(INITR_REDTAB);						// initializes the ST7735 screen
	short selAvg=6;												  // selects ADC averaging rate
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
	ADC0_InitSWTriggerSeq3_Ch9(selAvg);	  // uses 1 times sampling rate on the ADC
	Timer1_Init();												// Sets up Timer1 count for ~53 seconds to time duration in between ADC reads
  Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
  GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000; // configure PF2 as GPIO
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  PF2 = 0;                              // turn off LED
	EnableInterrupts();
  while(1){
    GPIO_PORTF_DATA_R ^= 0x02;  				//toggles PF1 so we know when main program is running
		if(dataFlag == 1){
			jitter=Jitter_Calc(ADCTime);
			PMF_Create(ADCData,pmf_array, selAvg);
			dataFlag++;
		}
  }
}


	

