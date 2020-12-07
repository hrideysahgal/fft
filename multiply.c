#include <math.h>
#include <stdlib.h>

#include "fft.h"
#include "multiply.h"


unsigned int pow2_larger(const unsigned int size) {
	unsigned int pow2 = 1;
	while (pow2 <= size) {
		pow2 *= 2;
	}
	return pow2;
}

void multiply(COMPLEX num1_complex[], COMPLEX num2_complex[], const unsigned int size) {
	fft(num1_complex, size, false);
	fft(num2_complex, size, false);
	for (int idx = 0; idx < size; ++idx) {
		num1_complex[idx] *= num2_complex[idx];
	}
	fft(num1_complex, size, true);
}