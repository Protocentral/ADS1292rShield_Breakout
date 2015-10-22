#include <FIR.h>
#define FILTERTAPS 5
FIR fir;

void setup() {

	Serial.begin(115200);	// open the serial port, I like them fast ;-)

	// declare variables for coefficients
	// these should be calculated by hand, or using a tool
	// in case a phase linear filter is required, the coefficients are symmetric
	// for time optimization it seems best to enter symmetric values like below
	float coef[FILTERTAPS] = { 0.021, 0.096, 0.146, 0.096, 0.021};
	fir.setCoefficients(coef);

	  //declare gain coefficient to scale the output back to normal
	float gain = 0.38; // set to 1 and input unity to see what this needs to be
	fir.setGain(gain);
}

void loop() {

	// declare input and output variables
	float input = 1; // without a real input, looking at the step respons (input at unity, 1) would be nice to see
	float output = 0; // output as a 0, but that doesn't really matter

	// This is the loop that takes care of calling the FIR filter for some samples

	for (byte n = 0; n < FILTERTAPS + 2; n++) { 		// If you like to see the step response, take at least as many cycles as the length of your FIR filter (FILTERTAPS + 1 or 2)
		Serial.print("n= ");		// print the sample number
		Serial.println(n, DEC);
		Serial.println("Now calling fir...");
		output = fir.process(input);		// here we call the fir routine with the input. The value 'fir' spits out is stored in the output variable.
		Serial.print("fir presented the following value= ");
		Serial.println(output);		// just for debugging or to understand what it does, print the output value
	}

	while (true) {};			// endless loop
}