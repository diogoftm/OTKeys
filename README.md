## OTKeys
This repository implements a string Oblivious Transfer aimed to be used in the application layer of Quantum Key Distribution System that supports oblivious keys generation. This library is currently mainly being used by [QMP-SPDZ](https://github.com/diogoftm/QMP-SPDZ) for Secure Multiparty Computation (SMC).

For an overview of key-based OT protocols, check the following two references:
- [Quantum oblivious transfer: a short review](https://www.mdpi.com/1099-4300/24/7/945)
- [Generation and Distribution of Quantum Oblivious Keys for Secure Multiparty Computation](https://www.mdpi.com/2076-3417/10/12/4080)

In the `main` branch there's the version of the protocol that uses oblivious key retrieved from a quantum Key Management System (KMS) following the [ETSI QKD 014](https://www.etsi.org/deliver/etsi_gs/QKD/001_099/014/01.01.01_60/gs_qkd014v010101p.pdf) standard.

In the `etsi_004` branch there's the version of the protocol that uses oblivious key retrieved from a quantum Key Management System (KMS) following the [ETSI QKD 004](https://www.etsi.org/deliver/etsi_gs/QKD/001_099/004/02.01.01_60/gs_qkd004v020101p.pdf) standard from [this](https://forge.etsi.org/rep/qkd/gs004-app-int/-/tree/edition3?ref_type=heads) reference implementation.

In the `simulated_ok_gen` branch a version using a local key generator simulator can be found. 

## Implementation

This implementation is divided in two different programs, one for the sender and another for the receiver.
Each of the programs contains a structure for storing the relevant data for each party, as well as a set of functions.
These functions are called from a main program and perform the local operations to be done by each party.

## Compilation & test

First, make sure to set the location of the certificates for your SAE and the KMS URI in the `ENV.sh` file.

A simulated KMS that can be found in [this repository](https://github.com/diogoftm/simulated-kms).

Then, setup the environment and compile:
```bash
source ENV.sh
make
```

To run the test executable:
```bash
./uirotk_test
```
The test program might not work out of the box because of the SAE IDs set previously, so make sure that all certificates are set and that they match the specified SAE IDs in `src/ui_rotk/main_uirotk.c`
