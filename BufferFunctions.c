#include <stdint.h>
#include "ST7735.h"

//static short PMFArray[4096];

short Jitter_Calc(int* ADCTime){ //can edit input parameter if we move buffer back to main
	short maxDiff=0;
  for(int i=0;i<999;i++){
	  if(abs(ADCTime[i]-ADCTime[i+1])>maxDiff){
		  maxDiff=abs(ADCTime[i]-ADCTime[i+1]);
		}
	}
	return maxDiff;
}

short* PMF_Create(int* ADCData, short* PMFArray){
	short adcRange;
	short adcMin;
	short adcMax;
  for(int i=0;i<1000;i++){
		PMFArray[ADCData[i]]+=1;
  }
	int i=0;
	while(PMFArray[i]==0){
		i++;
	}
	adcMin=PMFArray[i];
	i=4095;
	while(PMFArray[i]==0){
		i--;
	}
	adcMax=PMFArray[i];
	adcRange=adcMax-adcMin;
/*
	ST7735_SetCursor();
  ST7735_OutUDec();
  ST7735_OutString();
  ST7735_PlotClear();
  ST7735_PlotBar();
  ST7735_PlotNext();
*/
	
  return PMFArray;
}
