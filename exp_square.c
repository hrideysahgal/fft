#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "multiply.h"

typedef unsigned long long BIG;

BIG usecs(const struct timeval t) {
	return (BIG) t.tv_sec * 1000000 + t.tv_usec;
}

BIG get_num(const COMPLEX num_complex[], const unsigned int size) {
	BIG num = 0, place = 1;
	for (unsigned int idx = 0; idx < size; ++idx) {
		num += round(num_complex[idx]) * place;
		place *= 10;
	}
	return num;
}

void populate_complex(BIG num, COMPLEX num_complex[]) {
	unsigned int idx = 0;
	while (num) {
		num_complex[idx++] = num % 10;
		num /= 10;
	}
}

unsigned int get_size(BIG num) {
	unsigned int size = 0;
	while (num) {
		num /= 10;
		size++;
	}
	return size;
}

BIG multiply_nums(BIG num1, BIG num2) {
	unsigned int size = pow2_larger(get_size(num1) + get_size(num2));
	COMPLEX *num1_complex = (COMPLEX *) calloc(size, sizeof(COMPLEX));
	COMPLEX *num2_complex = (COMPLEX *) calloc(size, sizeof(COMPLEX));
	populate_complex(num1, num1_complex);
	populate_complex(num2, num2_complex);

	multiply(num1_complex, num2_complex, size);
	BIG answer = get_num(num1_complex, size);

	free(num1_complex);
	free(num2_complex);
	return answer;
}

void experiment(FILE *file, BIG nums[], const unsigned int size, const unsigned int reps) {
	fprintf(file, "num, c_res, fft_res, c_usecs, fft_usecs\n");
	for (unsigned int iter = 0; iter < size; ++iter) {
		for (unsigned int rep = 0; rep < reps; ++rep) {
			struct timeval start, end, c, fft;
			gettimeofday(&start, NULL);
			BIG c_res = nums[iter] * nums[iter];
			gettimeofday(&end, NULL);
			timersub(&end, &start, &c);

			gettimeofday(&start, NULL);
			BIG fft_res = multiply_nums(nums[iter], nums[iter]);
			gettimeofday(&end, NULL);
			timersub(&end, &start, &fft);

			fprintf(file, "%lld, %lld, %lld, %lld, %lld\n", nums[iter], c_res, fft_res, usecs(c), usecs(fft));
		}
	}
}

int main(const int argc, const char* argv[]) {
	if (argc < 4) {
		printf("Usage: ./multiply file reps num1 [num2 num3 ...]\n");
		return 1;
	}

	BIG *nums = (BIG *) calloc(argc - 3, sizeof(BIG));
	for (int arg = 3; arg < argc; ++arg) {
		if (argv[arg][0] == '-') {
			printf("Numbers must be positive.\n");
			free(nums);
			return 1;
		}

		nums[arg - 3] = strtoull(argv[arg], NULL, 10);
	}

	FILE *file = fopen(argv[1], "w+");
	experiment(file, nums, argc - 3, atoi(argv[2]));
	free(nums);
	fclose(file);
	return 0;
}