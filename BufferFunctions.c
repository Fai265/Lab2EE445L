#include <stdint.h>
static int ADCTime[1000];
static int ADCData[1000];
static short PMFArray[4096];

short Jitter_Calc(){ //can edit input parameter if we move buffer back to main
	short maxDiff=0;
  for(int i=0;i<999;i++){
	  if((ADCTime[i]-ADCTime[i+1])>maxDiff){
		  maxDiff=ADCTime[i]-ADCTime[i+1];
		}
	}
	return maxDiff;
}

short* PMF_Create(){
  for(int i=0;i<1000;i++){
		PMFArray[ADCData[i]]+=1;
  }
return PMFArray;
}