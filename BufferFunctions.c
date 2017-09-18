#include <stdint.h>
#include "ST7735.c"

//static int PMFArray[4096];

int Jitter_Calc(int* ADCTime){ //can edit input parameter if we move buffer back to main
	int maxDiff=0;
  for(int i=0;i<999;i++){
	  if(abs(ADCTime[i]-ADCTime[i+1])>maxDiff){
		  maxDiff=abs(ADCTime[i]-ADCTime[i+1]);
		}
	}
	return maxDiff;
}

int* PMF_Create(int* ADCData, int* PMFArray){
	int adcRange;
	int xDiff = 1000;
	uint32_t maxCount = 0;
	int numHatch = 4;
	int adcMin;
	int adcMax;

  //for(int i=0;i<1000;i++){
  for(int i=0;i<4096;i++){
		//PMFArray[ADCData[i]]++;
		//if(PMFArray[ADCData[i]] > maxCount){
		if(PMFArray[i] > maxCount){
			maxCount = PMFArray[ADCData[i]];
			maxCount=PMFArray[i];
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
	char max[5];
	char min[5];
  sprintf(max,"%d",adcMax);
	sprintf(min,"%d",adcMin);
	int pmfDisplayArray[128];
	ST7735_OutString("0x sampling\n\n");
	ST7735_OutString("Range: ");
	ST7735_OutString(min);
	ST7735_OutString(" to ");
	ST7735_OutString(max);
	ST7735_PlotClear(0, maxCount);
	
	if(adcRange>=128){
		for(int i = adcMin; i <= adcMax; i += 2){
			pmfDisplayArray[i] = PMFArray[i] + PMFArray[i+1];
			if(adcRange >= 256){
				pmfDisplayArray[i] += PMFArray[i+2] + PMFArray[i+3];
			}
			if(pmfDisplayArray[i] > maxCount){
				maxCount = pmfDisplayArray[i];
			}
			if(adcRange >= 256){
				i += 2;
			}
		}
		for(int i = 0; i < 128; i++){
			ST7735_PlotBar(pmfDisplayArray[i]);
			ST7735_PlotNext();
		}
	}
	else{
		int offset = 128/adcRange;
		for(int i = adcMin; i <= adcMax; i++){
			ST7735_PlotBar(PMFArray[i]);
			ST7735_PlotNext();
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
