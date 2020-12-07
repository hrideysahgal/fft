#include <stdbool.h>
#include <complex.h>
#include <math.h>

#include "fft.h"


void fft(COMPLEX coeffs[], const unsigned int n, const bool inverse) {
	if (n == 1) {
		return;
	}

	COMPLEX even[n / 2], odd[n / 2];
	for (unsigned int idx = 0; idx * 2 < n; ++idx) {
		even[idx] = coeffs[idx * 2];
		odd[idx] = coeffs[idx * 2 + 1];
	}

	fft(even, n / 2, inverse);
	fft(odd, n / 2, inverse);

	double angle = (inverse ? -1 : 1) * 2 * M_PI / n;
	COMPLEX w = 1;
	COMPLEX wn = CMPLX(cos(angle), sin(angle));

	for (unsigned int idx = 0; idx * 2 < n; ++idx) {
		coeffs[idx] = even[idx] + w * odd[idx];
		coeffs[idx + n / 2] = even[idx] - w * odd[idx];

		if (inverse) {
			coeffs[idx] /= 2;
			coeffs[idx + n / 2] /= 2;
		}

		w *= wn;
	}
}