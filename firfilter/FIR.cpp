#include "FIR.h"

FIR::FIR() {
  k = 0; //initialize so that we start to read at index 0
  for (int i=0; i<FILTERTAPS; i++) {
    values[i] = 0; // to have a nice start up, fill the array with 0's
  }
  //TODO calculate default gain?
  //TODO calculate default coefs?
}
//construct with coefs
FIR::FIR(float newGain, float *newCoefs) {
  k = 0; //initialize so that we start to read at index 0
  setGain(newGain);
  for (int i=0; i<FILTERTAPS; i++) {
    values[i] = 0; // to have a nice start up, fill the array with 0's
  }
  setCoefficients(newCoefs);
}

void FIR::setGain(float newGain) {
  gain = newGain;
}

void FIR::setCoefficients(float *newCoefs) {
  for (int i=0; i<FILTERTAPS; i++) {
    coef[i] = newCoefs[i];
  }
}
//set coefficient at specified index
void FIR::setCoefficient(int idx, float newCoef) { 
  coef[idx] = newCoef; 
}

float FIR::process(float in) {
  float out = 0;				// out is the return variable. It is set to 0 every time we call the filter!

  values[k] = in;				// store the input of the routine (contents of the 'in' variable) in the array at the current pointer position

    for (int i=0; i<FILTERTAPS; i++) {					// we step through the array
    out += coef[i] * values[(i + k) % FILTERTAPS];	// ... and add and multiply each value to accumulate the output
    //  (i + k) % filterTaps creates a cyclic way of getting through the array
  }

  out /= gain;				// We need to scale the output (unless the coefficients provide unity gain in the passband)

  k = (k+1) % FILTERTAPS;		// k is increased and wraps around the filterTaps, so next time we will overwrite the oldest saved sample in the array

  return out;					// we send the output value back to whoever called the routine
}

