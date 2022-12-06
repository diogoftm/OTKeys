## Using with libScapi and MPC-Benchmark/SemiHonestYao

To use this implementation together with the libscapi implementation, please follow these steps:

1. Insert the folder `quantum_random_oblivious_transfer` inside `libscapi/lib/OTExtensionBristol`

2. Update `Makefile` in `libscapi/lib/OTExtensionBristol`:

	- Create `LIBQOKDOT` path variable to `libqokdot.a` library:

	```
	12	#LIBSIMPLEOT = SimpleOT/libsimpleot.a
	13	LIBQOKDOT = quantum_random_oblivious_transfer/libqokdot.a
	```
	
	- Make `quantum_random_oblivious_transfer` project

	
	```
	19	#@echo "compling simple ot..........................."
	20	#cd SimpleOT && make
	21	cd quantum_random_oblivious_transfer && make	
	22	@echo "compling .a ...................."
	```

	- Install `libqokdot.a` instead of `libsimpleot.a`

	```
	33 	install -m 0644 ${LIBQOKDOT} $(libdir)
	34	#install -m 0644 ${LIBSIMPLEOT} $(libdir)
	```

3. Update `BaseOT.cpp` in `libscapi/lib/OTExtensionBristol/OT`:

	- Adapt to `qot_receiver.h` and `qot_sender.h`:

	```
	14	//#include "SimpleOT/ot_sender.h"
	15	//#include "SimpleOT/ot_receiver.h"
	16	#include "quantum_random_oblivious_transfer/qot_receiver.h"
	17	#include "quantum_random_oblivious_transfer/qot_sender.h"
	```

	- Adapt function `BaseOT::exec_base`:
		- Insert 'bool new_receiver_inputs' variable
		- Change object `SIMPLEOT_SENDER/RECEIVER` to `QKDOT_SENDER/RECEIVER`
		- Reformulate the function as shown in the doc

4. Update `BaseOT.h` in `libscapi/lib/OTExtensionBristol/OT`:

	- Add 'bool new_receiver_inputs' variable inside `virtual void exec_base` function:

	```
	59	virtual void exec_base(bool new_receiver_inputs=true)
	```

5. Update `CMakeList.txt` in `MPC-Benchmark/SemiHonestYao`:

	- Change libscapi to qlibscapi:

```
	6	INCLUDE_DIRECTORIES($ENV{HOME} $ENV{HOME}/qlibscapi/install/include
	7		$ENV{HOME}/qlibscapi/lib/OTExtensionBristol
	8		$ENV{HOME}/qlibscapi/install/include
	9		$ENV{HOME}/qlibscapi/install/include/libOTe/cryptoTools/
	10		$ENV{HOME}/qlibscapi/install/include/libOTe)
```

```
	15	link_directories($ENV{HOME}/qlibscapi/install/lib)
```

```
	22	#OTExtensionBristol libsimpleot.a libOTe.a libcryptoTools.a libmiracl.a
	23	OTExtensionBristol libqokdot.a libOTe.a libcryptoTools.a libmiracl.a
```

6. Update `YaoParties.hpp` in `MPC-Benchmark/SemiHonestYao`

```
	60	#include <qlibscapi/include/infra/Scanner.hpp>
	61	#include <qlibscapi/include/infra/ConfigFile.hpp>
	62	//#include <libscapi/include/infra/Scanner.hpp>
	63	//#include <libscapi/include/infra/ConfigFile.hpp>
```

