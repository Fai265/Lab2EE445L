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

int* PMF_Create(int* ADCData, int* PMFArray, int sampleRate){
	ST7735_SetCursor(0, 0);
	int sampling = 1;
	for(int i = 0; i < sampleRate; i++){
		sampling *= 2;
	}
	int adcRange;
	int xDiff = 1000;
	uint32_t maxCount = 0;
	int numHatch = 4;
	int adcMin;
	int adcMax;
	int modeIndex;
  for(int i=0;i<1000;i++){
  //for(int i=0;i<4096;i++){
		PMFArray[ADCData[i]]++;
		if(PMFArray[ADCData[i]] > maxCount){
		//if(PMFArray[i] > maxCount){
			modeIndex=ADCData[i];
			maxCount = PMFArray[ADCData[i]];
			//maxCount=PMFArray[i];
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
	
	char samp[2];
	char max[5];
	char min[5];
	char graphMin[5];
	char graphMax[5];
	int pmfDisplayArray[128];
	
	if(adcRange>=128){
		short upperLimit = 256;
		if(adcRange > 256){
			upperLimit = 512;
		}
		
		for(int i = 0; i < upperLimit; i += 2){
			int arraySizer=i/(2*(upperLimit/256)); //index into PMFDisplay changes based on range of values
			int pmfIndex=(modeIndex+i-(upperLimit/2)); //index into PMFArray goes halfway above and below the mode
			
			pmfDisplayArray[arraySizer] = PMFArray[pmfIndex] + PMFArray[pmfIndex+1]; //(2*(upperLimit/256)) for when dividing by 4
			if(adcRange >= 256){
				pmfDisplayArray[i/4] += PMFArray[pmfIndex+2] + PMFArray[pmfIndex+3];
			}
			if(pmfDisplayArray[arraySizer] > maxCount){
				maxCount = pmfDisplayArray[arraySizer];
			}
			if(adcRange >= 256){
				i += 2;
			}
		}
		ST7735_PlotClear(0, maxCount);
		sprintf(max,"%d",adcMax);
	  sprintf(min,"%d",adcMin);
		sprintf(samp, "%d", sampling);
		sprintf(graphMin,"%d",modeIndex-upperLimit/2);
		sprintf(graphMax,"%d",modeIndex+upperLimit/2);
		ST7735_OutString(samp);
		ST7735_OutString("x sampling\n");
		ST7735_OutString("ADC: ");
		ST7735_OutString(min);
		ST7735_OutString(" to ");
		ST7735_OutString(max);
		ST7735_OutString("\n");
		ST7735_OutString("Graph: ");
		ST7735_OutString(graphMin);
		ST7735_OutString(" to ");
		ST7735_OutString(graphMax);
		for(int i = 0; i < 128; i++){
			ST7735_PlotBar(pmfDisplayArray[i]);
			ST7735_PlotNext();
		}
	}
	else{
		ST7735_PlotClear(0, maxCount);
		sprintf(max,"%d", adcMax);
	  sprintf(min,"%d",adcMin);
		sprintf(samp, "%d", sampling);
		sprintf(graphMin,"%d",modeIndex-64);
		sprintf(graphMax,"%d",modeIndex+64);
		ST7735_OutString(samp);
		ST7735_OutString("x sampling\n");
		ST7735_OutString("ADC: ");
		ST7735_OutString(min);
		ST7735_OutString(" to ");
		ST7735_OutString(max);
		ST7735_OutString("\n");
		ST7735_OutString("Graph: ");
		ST7735_OutString(graphMin);
		ST7735_OutString(" to ");
		ST7735_OutString(graphMax);

		int offset = 128/adcRange;
		for(int i = modeIndex-64; i <= modeIndex+64; i++){
			ST7735_PlotBar(PMFArray[i]);
			ST7735_PlotNext();
		}
	}
	

  return PMFArray;
}
