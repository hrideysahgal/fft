#ifndef FFT_H
#define FFT_H

#include <stdbool.h>
#include <complex.h>

typedef long double complex COMPLEX;


void fft(COMPLEX coeffs[], const unsigned int n, const bool inverse);

#endif