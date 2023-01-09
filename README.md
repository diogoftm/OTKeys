(C) University of Lisbon - IST

Mariana Gama and Manuel B Santos

## OTKeys

This repo implements a string Oblivious Transfer based on two types of keys:

1. Oblivious keys (`ok`).
2. Random oblivious transfer keys (`rotk`).

For an overview of key-based OT protocols, check the following two references [1],[2].

The repo aims to support two key format: binary format (`b`) and unsigned int format (`ui`). The system is intended to be used along with a quantum oblivious key distribution system. However, for convenience, we provide a simulator that generates keys in the desired format.


## Simulation

The python simulator is in `simulator/simulator.py`. Currently, it supports the following format:


|           | ui_rotk | b_rotk | ui_ok | b_ok |
|-----------|:-------:|:------:|:-----:|:----:|
| Supported |   Yes   |   No   |   No  |  No  |

It generates keys to the `keys` folder according to the following convention name: 

`<party>_myId<i>_otherId<j>_<format>.txt`

 where `<party> = receiver` or `sender`, `<i>` and `<j>` (IDs of each party where 0 <= i,j < number of parties) and `<format> = uirotk`, `brotk`, `uiok` or `bok`.


## Implementation

This implementation is divided in two different programs, one for the sender and another for the receiver. Currently, it supports the following format:

| format   | ui_rotk | b_rotk | ui_ok | b_ui |
|----------|:-------:|:------:|:-----:|:----:|
| sender   |   Yes   |   No   |   No  |  No  |
| receiver |   Yes   |   No   |   No  |  No  |


Each of the programs contains a structure for storing the relevant data for each party, as well as a set of functions.
These functions are called from a main program and perform the local operations to be done by each party. 

The universal hash function is done following the strategy presented in this [blog](https://lemire.me/blog/2018/08/15/fast-strongly-universal-64-bit-hashing-everywhere/)

In `src/<format>/main_*.c`, there is a test execution of the sender and receiver programs, with the outputs being printed at the end. The test executable is saved inside `bin` forlder.

### Contribute

If you want to add a new format, use the following conventions:

1. Create a folder named `<format>` in the `src` and `include` folders.
2. Create two files `.c` and `.h` with the following format: `<party>_<format>`, where `<party> = receiver` or `sender` and `<format> = uirotk`, `brotk`, `uiok` or `bok`.
3. Create a `main_<format>.c` file in the `src` folder.
4. Adapt the `Makefile` file to build a static librar `.a` as `lib<format>.a` and a test executable called `<format>_test` inside `bin` folder.


## Compilation 

Execute the `make` command to generate all the associated static libraries `lib<format>.a` supported, as well as all `<format>_test` (test program generated from `main_<format>.c`).

If you just want to execute one of the formats supported, execute `make <format>`.

### Run

To run a test executable (e.g. uirotk_test) does the following:

```
$ cd bin
$ ./uirotk_test
```


#### Remarks


We are considering that the oblivious key length is 512, and the OT ouput length is 128. These values were chosen because of the OT length used in the libscapi/MASCOT, and also because of the relation between these lengths that is required for security (see MASCOT paper).

These lengths are defined in the sender and receiver header files, and might be changed if needed. Note, however, that the hash function is written such that it generates outputs with half the size of the input (the hash function input size is always half of the oblivious key length! In this case, the hash function input size is 256 and the hash function output size is 128). Hence, depending on the desired oblivious key length and OT output length, you might need to change the way that the hash function is written (you can find it in the last loop of the `receiver_output` and `sender_output` functions).

In this version of the program, both the receiver and the sender are reading their keys from the `keys` folder that is to be placed in the functionality's folder. Before running the program, check that the location of these files is correctly linked in `qot_receiver.c` and `qot_sender.c`.
If you want to run the oblivious key simulator for each run of this OT, we suggest calling it at the beginning of the `receiver_okd` and `sender_okd` functions.

In this project, there are two folders named `libscapi_files` and `mascot_files`. In those folders, there are the adaptations of the Libscapi/MASCOT makefile (which links the `lib<format>.a` library when compiling the Libscapi/MASCOT) and also of the `BaseOT.cpp` file. This last file contains the method for doing base OTs for Libscapi/MASCOT. In the original Libscapi/MASCOT implementation, these OTs are done using the SimpleOT protocol. With this new version of the BaseOT file, the OTs will be done using the Quantum Random OT that we implemented. 


## References

[1] [Quantum oblivious transfer: a short review](https://www.mdpi.com/1099-4300/24/7/945)

[2] [Generation and Distribution of Quantum Oblivious Keys for Secure Multiparty Computation](https://www.mdpi.com/2076-3417/10/12/4080)
