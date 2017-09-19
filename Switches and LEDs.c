#include <stdio.h>
#include <stdint.h>
#include "tm4c123gh6pm.h"

#define PF4							(*((volatile uint32_t *)0x40025040))

void DelayWait10ms(uint32_t n){
	uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
          time--;
    }
    n--;
  }
}

void Pause(void){
	while(PF4==0x00){
		DelayWait10ms(10);
	}
	while(PF4==0x10){
		DelayWait10ms(10);
	}
}

void PF4Switch_Init(){
  SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF4
  GPIO_PORTF_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x10;      // 4) disable analog function on PF4
  GPIO_PORTF_PUR_R |= 0x10;         // 5) pullup for PF4
  GPIO_PORTF_AFSEL_R &= ~0x10;      // 6) regular port function
  GPIO_PORTF_DEN_R |= 0x10;         // 7) enable digital port
	}