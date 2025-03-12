## OTKeys
This repository implements a string Oblivious Transfer aimed to be used in the application layer of Quantum Key Distribution System that supports oblivious keys generation. This library is currently mainly being used by [QMP-SPDZ](https://github.com/diogoftm/QMP-SPDZ) for Secure Multiparty Computation (SMC).

For an overview of key-based OT protocols, check the following two references:
- [Quantum oblivious transfer: a short review](https://www.mdpi.com/1099-4300/24/7/945)
- [Generation and Distribution of Quantum Oblivious Keys for Secure Multiparty Computation](https://www.mdpi.com/2076-3417/10/12/4080)

In the `main` branch there's the version of the protocol that uses oblivious key retrieved from a quantum Key Management System (KMS) following the [ETSI QKD 004](https://www.etsi.org/deliver/etsi_gs/QKD/001_099/014/01.01.01_60/gs_qkd014v010101p.pdf) standard.

In this branch (`etsi_004`) there's the version of the protocol that uses oblivious key retrieved from a quantum Key Management System (KMS) following the [ETSI QKD 004](https://www.etsi.org/deliver/etsi_gs/QKD/001_099/004/02.01.01_60/gs_qkd004v020101p.pdf) standard from [this](https://forge.etsi.org/rep/qkd/gs004-app-int/-/tree/edition3?ref_type=heads) reference implementation.

In the `simulated_ok_gen` branch a version using a local key generator simulator can be found. 

## Implementation

This implementation is divided in two different programs, one for the sender and another for the receiver.
Each of the programs contains a structure for storing the relevant data for each party, as well as a set of functions.
These functions are called from a main program and perform the local operations to be done by each party. 

In `src/<format>/main_*.c`, there is a test execution of the sender and receiver programs, with the outputs being printed at the end.

## Compilation & test

First, update the `ENV.sh` file to have the right location of the certificates, keys and ID for your SAE, the KMS URI, and a strict role for the key requesting in case there only one source of keys with a specific role. 

You can generate a self-signed certificate in the `ssl` directory using the `generate_ca_and_selfsigned_cert` script. Nevertheless, in a real scenario this operation should be made on the KMS side.

Then, export the environment variables and compile:
```bash
source ENV.sh
make
```

To test run:
```bash
./uirotk_test
```
