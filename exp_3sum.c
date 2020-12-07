#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "multiply.h"

long usecs(const struct timeval t) {
	return (long) t.tv_sec * 1000000 + t.tv_usec;
}

int compare_int(const void *a, const void *b) {
	return (*(int *) a) - (*(int *) b);
}

unsigned int three_sum(int nums[], const unsigned int size) {
	unsigned int triplets = 0;
	qsort(nums, size, sizeof(int), compare_int);
	for (unsigned int idx = 0; idx < size; ++idx) {
		if (idx != 0 && nums[idx - 1] == nums[idx]) {
			continue;
		}

		unsigned int lo = idx + 1, hi = size - 1;
		while (lo < hi) {
			int sum = nums[idx] + nums[lo] + nums[hi];
			if (sum < 0) {
				++lo;
			} else if (sum > 0) {
				--hi;
			} else {
				triplets++;
				++lo;
				--hi;
			}
		}
	}
	return triplets;
}

unsigned int three_sum_fft(int nums[], const unsigned int size, const unsigned int abs_range) {
	unsigned int triplets = 0;
	qsort(nums, size, sizeof(int), compare_int);

	const unsigned int n = 2 * (2 * abs_range + 1);
	unsigned int pow2 = pow2_larger(n);
	COMPLEX *complex0 = (COMPLEX *) calloc(pow2, sizeof(COMPLEX));
	COMPLEX *complex1 = (COMPLEX *) calloc(pow2, sizeof(COMPLEX));
	COMPLEX *complex2 = (COMPLEX *) calloc(pow2, sizeof(COMPLEX));
	for (unsigned int idx = 0; idx < size; ++idx) {
		if (nums[idx]) {
			complex0[abs_range + nums[idx]] = round(complex0[abs_range + nums[idx]]) + 1;
		}
	}
	memcpy(complex1, complex0, pow2 * sizeof(COMPLEX));
	memcpy(complex2, complex0, pow2 * sizeof(COMPLEX));

	multiply(complex1, complex2, pow2);

	for (unsigned int idx = 0; idx < n; ++idx) {
		if (idx % 2 == 0) {
			complex1[idx] = round(complex1[idx]) - round(complex0[idx / 2]);
		}
	}

	for (unsigned int idx = 0; idx < n / 2; ++idx) {
		triplets += round(complex1[idx + abs_range]) * round(complex0[idx]) / 2;
	}

	free(complex0);
	free(complex1);
	free(complex2);
	return triplets;
}

void experiment(FILE *file, int nums[], const unsigned int size, const unsigned int reps, const unsigned int abs_max, const unsigned int u_max) {
	fprintf(file, "u, n2_res, fft_res, n2_usecs, fft_usecs\n");
	for (unsigned int u = abs_max; u <= u_max; ++u) {
		for (unsigned int rep = 0; rep < reps; ++rep) {
			struct timeval start, end, n2, fft;
			gettimeofday(&start, NULL);
			int n2_res = three_sum(nums, size);
			gettimeofday(&end, NULL);
			timersub(&end, &start, &n2);

			gettimeofday(&start, NULL);
			int fft_res = three_sum_fft(nums, size, u);
			gettimeofday(&end, NULL);
			timersub(&end, &start, &fft);

			fprintf(file, "%d, %d, %d, %ld, %ld\n", u, n2_res, fft_res, usecs(n2), usecs(fft));
		}
	}
}

int main(const int argc, const char* argv[]) {
	if (argc < 7) {
		printf("Usage: ./3sum file reps u_max num1 num2 num3 [num4 num5 ...]\n");
		return 1;
	}

	const unsigned int u_max = atoi(argv[3]);
	int abs_max = INT_MIN;
	int *nums = (int *) calloc(argc - 4, sizeof(int));
	for (int arg = 4; arg < argc; ++arg) {
		nums[arg - 4] = atoi(argv[arg]);
		abs_max = fmax(abs_max, abs(atoi(argv[arg])));
	}

	if (u_max < abs_max) {
		printf("u_max must be >= max(abs(nums)).");
		free(nums);
		return 1;
	}

	FILE *file = fopen(argv[1], "w+");
	experiment(file, nums, argc - 4, atoi(argv[2]), abs_max, u_max);
	free(nums);
	fclose(file);
	return 0;
}