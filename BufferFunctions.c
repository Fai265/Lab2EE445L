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
	short xDiff = 1000;
	uint32_t maxCount = 0;
	short numHatch = 4;
	short adcMin;
	short adcMax;
  for(int i=0;i<1000;i++){
		PMFArray[ADCData[i]]+=1;
		if(PMFArray[ADCData[i]] > maxCount){
			maxCount = PMFArray[ADCData[i]];
		}
  }
	int i=0;
	while(PMFArray[i]==0 && i < 4096){
		i++;
	}
	adcMin=i;
	i=4095;
	while(PMFArray[i]==0 && i >= 0){
		i--;
	}
	adcMax=i;
	adcRange=adcMax-adcMin;
	
	/*
	if(adcRange < 1000){
		if(adcRange < 100){
			xDiff = 10;
			numHatch = adcRange / 10;
		}
		else{
			xDiff = 100;
			numHatch = adcRange / 100;
		}
	}
	*/
	short pmfDisplayArray[160];
	if(adcRange >= 160){
		short offset = adcRange/160;
		short k = 0;
		for(int i = adcMin; i <= adcMax; i += offset){
			for(int j = 0; j < offset; j++){
				pmfDisplayArray[k] += PMFArray[i+j];
			}
			if(pmfDisplayArray[k] > maxCount){
				maxCount = pmfDisplayArray[k];
			}
			k++;
		}
		ST7735_PlotClear(0, maxCount);
		for(int i = 0; i < 160; i++){
			ST7735_PlotBar(pmfDisplayArray[i]);
			ST7735_PlotNext();
	}
	}else{
		short offset = 160/adcRange;
		ST7735_PlotClear(0, maxCount);
		for(int i = adcMin; i < adcMax; i++){
			ST7735_PlotBar(PMFArray[i]);
			for(int j = 0; j < offset; j++){
				ST7735_PlotNext();
			}
		}
	}
	
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
