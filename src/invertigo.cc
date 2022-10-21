#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <malloc.h>

#define N_ROWS 4
#define N_COLS 3

const char* input[N_ROWS][N_COLS] = {
	{		 "abcd",          "T",    "Celestial"},
	{"alewOwela",         "Me", "needABeerNow"},
	{		 "Anna", "AbsolutelY",     "doOrDont"},
	{		"this ",         "is",         "fun?"},
};

void invert_word(const char* in, char* out, size_t len);
void print_const(const char* in[N_ROWS][N_COLS], size_t rows, size_t cols);
void print_nonconst(char* in[N_ROWS][N_COLS], size_t rows, size_t cols);

int main(void) {
	printf("Input:\n");
	print_const(input, N_ROWS, N_COLS);
	printf("\n");

	char* output[N_ROWS][N_COLS];

	for (size_t row = 0; row < N_ROWS; ++row) {
		for (size_t col = 0; col < N_COLS; ++col) {
			size_t in_row    = N_ROWS - row - 1;
			size_t in_col    = N_COLS - col - 1;
			size_t in_size   = strlen(input[in_row][in_col]);
			output[row][col] = (char*)malloc(in_size);
			invert_word(input[in_row][in_col], output[row][col], in_size);
		}
	}
	printf("Output:\n");
	print_nonconst(output, N_ROWS, N_COLS);
	return 0;
}

void print_const(const char* in[][3], size_t rows, size_t cols) {
	for (size_t row = 0; row < rows; ++row) {
		for (size_t col = 0; col < cols; col++) {
			printf("%s ", in[row][col]);
		}
		printf("\n");
	}
}

void print_nonconst(char* in[][3], size_t rows, size_t cols) {
	for (size_t row = 0; row < rows; ++row) {
		for (size_t col = 0; col < cols; col++) {
			printf("%s ", in[row][col]);
		}
		printf("\n");
	}
}

void invert_word(const char* in, char* out, size_t len) {
	for (size_t i = 0; i < len; ++i) {
		out[i] = in[len - i - 1];
	}
	if (strcmp(in, out) == 0) {
		for (size_t i = 0; i < len; ++i) {
			out[i] = toupper(out[i]);
		}
	}
}