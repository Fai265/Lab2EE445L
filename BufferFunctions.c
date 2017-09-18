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
	int pmfDisplayArray[128];
	
	if(adcRange>=128){
		short upperLimit = 256;
		if(adcRange > 256){
			upperLimit = 512;
		}
		for(int i = 0; i < upperLimit; i += 2){
			pmfDisplayArray[i/(2*(upperLimit/256))] = PMFArray[i+adcMin] + PMFArray[i+adcMin+1]; //(2*(upperLimit/256)) for when dividing by 4
			if(adcRange >= 256){
				pmfDisplayArray[i/4] += PMFArray[i+adcMin+2] + PMFArray[i+adcMin+3];
			}
			if(pmfDisplayArray[i/(2*(upperLimit/256))] > maxCount){
				maxCount = pmfDisplayArray[i/(2*(upperLimit/256))];
			}
			if(adcRange >= 256){
				i += 2;
			}
		}
		ST7735_PlotClear(0, maxCount);
		sprintf(max,"%d",adcMin+upperLimit);
	  sprintf(min,"%d",adcMin);
		ST7735_OutString("0x sampling\n\n");
		ST7735_OutString("Range: ");
		ST7735_OutString(min);
		ST7735_OutString(" to ");
		ST7735_OutString(max);
		for(int i = 0; i < 128; i++){
			ST7735_PlotBar(pmfDisplayArray[i]);
			ST7735_PlotNext();
		}
	}
	else{
		ST7735_PlotClear(0, maxCount);
		sprintf(max,"%d",adcMin+128);
	  sprintf(min,"%d",adcMin);
		ST7735_OutString("0x sampling\n\n");
		ST7735_OutString("Range: ");
		ST7735_OutString(min);
		ST7735_OutString(" to ");
		ST7735_OutString(max);
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
