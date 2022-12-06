## Using with MASCOT

To use this implementation together with the MASCOT implementation provided in https://github.com/data61/MP-SPDZ/, please follow these steps:

1) Download the software provided in the link above, which contains the MASCOT implementation. After this step, you will have a folder `MP_SPDZ` which contains the original MASCOT implementation.

2) Before compiling the software, install the following requirements: `automake build-essential git libboost-dev libboost-thread-dev libsodium-dev libssl-dev libtool m4 python texinfo yasm`. Additionally, you might also need to download and install the MPIR library (see http://www.cs.sjsu.edu/~mak/tutorials/InstallMPIR.pdf and http://mpir.org/downloads.html. Use `--enable-cxx` flag when running configure).

3) Put the `quantum_random_oblivious_transfer` folder in the `MP-SPDZ` directory. Execute the `make` comand inside the folder `quantum_random_oblivious_transfer` to generate the `liboqokdot.a` library.

4) Substitute the `Makefile` in `MP_SPDZ` with the `Makefile` in `quantum_random_oblivious_transfer/mascot_files`, and the file `BaseOT.cpp` in `MP_SPDZ/OT` with the one with the same name in `quantum_random_oblivious_transfer/mascot_files`.

5) Check that the location of liboqokdot.a is correctly written in the Makefile in `MP_SPDZ`. Check that the location of `qot_receiver.h` and `qot_sender.h` is correctly written in `MP_SPDZ/OT/BaseOT.cpp`.

6) Compile the MASCOT by running the command `make -j8 mascot-party.x`. You can now use the MASCOT as described in https://github.com/data61/MP-SPDZ/. When the MASCOT is running, it will use this Quantum Random OT implementation whenever an OT is needed.


A guide for compiling and using this software is also provided in `main_tq.pdf`.


