#include <stdint.h>

// This function is used to calculate the jitter 
// in the ADC by reading the values in the ADCTime 
// array and search for the maximum difference between
// two consecutive time returns

int Jitter_Calc(); 


// This function takes in a data array and creates a histogram out of it
// Returns a pointer to an array of size 4096 with each
// memory address in the array functioning as 
short* PMF_Create();