The following quantum gates are implemented in C code.
```
pauli_x
pauli_y
pauli_z
hadamard
cnot
cphase	controlled phase
swap
cswap	controlled swap
ft	fourier transform
ift	inverse fourier transform
reduce	eigenstate reduction
```

To build and run
```
gcc order-finding-demo.c
./a.out
```

The result is a list of eigenstate probabilities.
