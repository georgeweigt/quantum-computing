// define LENGTH as 2^n where n is the number of qubits

#include <stdio.h>
#include <math.h>
#include <complex.h>

double complex psi[LENGTH]; // state vector
double p[LENGTH]; // probability vector

void
pauli_x(int n)
{
	int i, bitmask = 1 << n;
	double complex z;
	for (i = 0; i < LENGTH; i++)
		if (i & bitmask) {
			z = psi[i ^ bitmask];
			psi[i ^ bitmask] = psi[i];
			psi[i] = z;
		}
}

void
pauli_y(int n)
{
	int i, bitmask = 1 << n;
	double complex z0, z1;
	for (i = 0; i < LENGTH; i++)
		if (i & bitmask) {
			z0 = psi[i ^ bitmask];
			z1 = psi[i];
			psi[i ^ bitmask] = -I * z1;
			psi[i] = I * z0;
		}
}

void
pauli_z(int n)
{
	int i, bitmask = 1 << n;
	for (i = 0; i < LENGTH; i++)
		if (i & bitmask)
			psi[i] = -psi[i];
}

// multiple bits can be set in cbitmask to implement a toffoli gate

void
cnot(int n, int cbitmask)
{
	int i, bitmask = 1 << n;
	double complex z;
	for (i = 0; i < LENGTH; i++)
		if ((i & cbitmask) == cbitmask && (i & bitmask)) {
			z = psi[i ^ bitmask];
			psi[i ^ bitmask] = psi[i];
			psi[i] = z;
		}
}

void
hadamard(int n)
{
	int i, bitmask = 1 << n;
	double complex z0, z1;
	for (i = 0; i < LENGTH; i++)
		if (i & bitmask) {
			z0 = psi[i ^ bitmask];
			z1 = psi[i];
			psi[i ^ bitmask] = (z0 + z1)  / M_SQRT2;
			psi[i] = (z0 - z1) / M_SQRT2;
		}
}

// controlled phase (multiple bits can be set in cbitmask)

void
cphase(double complex z, int n, int cbitmask)
{
	int i, bitmask = 1 << n;
	for (i = 0; i < LENGTH; i++)
		if ((i & cbitmask) == cbitmask && (i & bitmask))
			psi[i] *= z;
}

// swap |00> = |00>
// swap |01> = |10>
// swap |10> = |01>
// swap |11> = |11>

void
swap(int m, int n)
{
	int i, mbit = 1 << m, nbit = 1 << n;
	double complex z;
	for (i = 0; i < LENGTH; i++)
		if ((i & mbit) && !(i & nbit)) {
			z = psi[i ^ mbit ^ nbit];
			psi[i ^ mbit ^ nbit] = psi[i];
			psi[i] = z;
		}
}

// controlled swap (multiple bits can be set in cbitmask)

void
cswap(int m, int n, int cbitmask)
{
	int i, mbit = 1 << m, nbit = 1 << n;
	double complex z;
	for (i = 0; i < LENGTH; i++)
		if ((i & cbitmask) == cbitmask && (i & mbit) && !(i & nbit)) {
			z = psi[i ^ mbit ^ nbit];
			psi[i ^ mbit ^ nbit] = psi[i];
			psi[i] = z;
		}
}

// fourier transform of qubits 0 to n - 1

void
ft(int n)
{
	int i, j;
	double complex z;
	for (i = n - 1; i >= 0; i--) {
		hadamard(i);
		for (j = 0; j < i; j++) {
			z = pow(0.5, i - j) * I * M_PI;
			z  = cexp(z);
			cphase(z, i, 1 << j);
		}
	}
	for (i = 0; i < n / 2; i++)
		swap(i, n - i - 1);
}

// inverse fourier transform of qubits 0 to n - 1

void
ift(int n)
{
	int i, j;
	double complex z;
	for (i = 0; i < n / 2; i++)
		swap(i, n - i - 1);
	for (i = 0; i < n; i++) {
		for (j = i - 1; j >= 0; j--) {
			z = -pow(0.5, i - j) * I * M_PI;
			z = cexp(z);
			cphase(z, i, 1 << j);
		}
		hadamard(i);
	}
}

// measure m qubits

void
measure(int m)
{
	int i, j, k, n;

	n = 1 << m; // n = 2^m

	// probabilities

	for (i = 0; i < LENGTH; i++)
		p[i] = psi[i] * conj(psi[i]);

	// sum over don't care bits

	k = LENGTH / n;

	for (i = 0; i < n; i++)
		for (j = 1; j < k; j++)
			p[i] += p[i + n * j];

	// histogram

	for (i = 0; i < n; i++) {

		// print eigenstate

		for (j = 0; j < m; j++)
			if (i & 1 << j)
				printf("1");
			else
				printf("0");

		// print probability

		printf(" %f ", p[i]);
		k = round(100.0 * p[i]);
		for (j = 0; j < k; j++)
			printf("*");
		printf("\n");
	}
}