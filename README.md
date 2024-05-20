## OTKeys
This repository implements a string Oblivious Transfer aimed to be used in the application layer of Quantum Key Distribution System that supports oblivious keys generation. This library is currently mainly being used by [QMP-SPDZ](https://github.com/diogoftm/QMP-SPDZ) for Secure Multiparty Computation (SMC).

For an overview of key-based OT protocols, check the following two references:
- [Quantum oblivious transfer: a short review](https://www.mdpi.com/1099-4300/24/7/945)
- [Generation and Distribution of Quantum Oblivious Keys for Secure Multiparty Computation](https://www.mdpi.com/2076-3417/10/12/4080)

In the `main` branch there's the version of the protocol that uses oblivious key retrieved from a quantum Key Management System (KMS) following the [ETSI QKD 004](https://www.etsi.org/deliver/etsi_gs/QKD/001_099/014/01.01.01_60/gs_qkd014v010101p.pdf) standard.

In the `etsi_004` brach there's the version of the protocol that uses oblivious key retrieved from a quantum Key Management System (KMS) following the [ETSI QKD 004](https://www.etsi.org/deliver/etsi_gs/QKD/001_099/004/02.01.01_60/gs_qkd004v020101p.pdf) standard from [this](https://forge.etsi.org/rep/qkd/gs004-app-int/-/tree/edition3?ref_type=heads) reference implementation.

In `simulated_ok_gen` a version using a local key generator simulator can be found. 

## Authors
Adapted by Diogo Matos (Instituto de Telecomunições - IT) from the work done by Mariana Gama and Manuel B Santos (University of Lisbon - IST).

## Implementation

This implementation is divided in two different programs, one for the sender and another for the receiver.
Each of the programs contains a structure for storing the relevant data for each party, as well as a set of functions.
These functions are called from a main program and perform the local operations to be done by each party. 

The universal hash function is done following the strategy presented in this [blog](https://lemire.me/blog/2018/08/15/fast-strongly-universal-64-bit-hashing-everywhere/)

In `src/<format>/main_*.c`, there is a test execution of the sender and receiver programs, with the outputs being printed at the end.

## Compilation 

First, make to set the location of the certificates for your SAE and the KMS URI. A simulated KMS can be found in [this repository](https://github.com/diogoftm/simulated-kms). 

Then, to compile run:
```bash
make
```

### Test

To run a test executable, `uirotk_test` make sure that all certificates are set and that they match the specificated SAE ids, if not replace them with the right ones.

#### Remarks

We are considering that the oblivious key length is 512, and the OT output length is 128. These values were chosen because of the OT length used in the libscapi/MASCOT, and also because of the relation between these lengths that is required for security.

These lengths are defined in the sender and receiver header files, and might be changed if needed. Note, however, that the hash function is written such that it generates outputs with half the size of the input (the hash function input size is always half of the oblivious key length! In this case, the hash function input size is 256 and the hash function output size is 128). Hence, depending on the desired oblivious key length and OT output length, you might need to change the way that the hash function is written (you can find it in the last loop of the `receiver_output` and `sender_output` functions).

In this project, there are two folders named `libscapi_files` and `mascot_files`. In those folders, there are the adaptations of the Libscapi/MASCOT makefile (which links the `lib<format>.a` library when compiling the Libscapi/MASCOT) and also of the `BaseOT.cpp` file. This last file contains the method for doing base OTs for Libscapi/MASCOT. In the original Libscapi/MASCOT implementation, these OTs are done using the SimpleOT protocol. With this new version of the BaseOT file, the OTs will be done using the Quantum Random OT that we implemented. 
