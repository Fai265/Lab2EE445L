#include <stdint.h>

// This function is used to calculate the jitter 
// in the ADC by reading the values in the ADCTime 
// array and searching for the maximum difference between
// two consecutive time returns
short Jitter_Calc(int* ADCTime); 


// This function takes in a data array and creates a PMF out of it
// Returns a pointer to an array of size 4096 with each
// memory address in the array functioning as 
int* PMF_Create(int* ADCData, int* PMFArray, int sampleRate);