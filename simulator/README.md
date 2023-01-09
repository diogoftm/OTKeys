## Oblivious key simulator

The idea of the simulator is simply to make the test easier and smoother. The simulator supports the following OT types:

1. `ui_rotk`: Unsigned int random oblivious transfer 


### Random oblivious transfer

This key type is simply an instantiation of random OT. Consider the following example:

Setting:
- Size of the oblivious key: 10 bits
- Alice (sender) - knows all bits
- Bob (receiver) - only knows half of the bits

Alice key: 01 10 10 01 11
Bob key:  00 01 10 11 01 

Let us consider blocks of two bits. Bob knows one of the bits from Alice's 2-bit blocks (green) and nothing about the other bit of Alice's 2-bit blocks (red). So, at each 2-bit block, Bob's key will tell him which bit he knows from the corresponding Alice's 2-bit block. This information is given by the first bit of Bob's 2-bit block (light blue colour). The second bit of Bob's 2-bit block is the bit he knows from the corresponding Alice's 2-bit block (green colour). Check below:

Alice key: 01 10 10 01 11
Bob key:  00 01 10 11 01 

## Run

The simulator is given by the `simulator.py` file. Run:

```
$ python3 simulator.py
```

It will generate oblivious keys (for the sender and the receiver) and saves inside `keys` folder. The default values of the oblivious keys specifications are as follows:

- Size of the key (`-s`): `512` bits
- Number of oblivious keys (`-n`): `128`
- Type of key (`-t`): `ui_rotk`
- Number of parties (`-p`): `2`

You can change these settings using the corresponding flags (`-s`, `-n`, `-t`, `-p`). 

### Naming

For each `<i>` and `<j>` (IDs of each party where 0 <= i,j < number of parties), it will generate two files with the following structure:

`receiver_myId<i>_otherId<j>_uirotk.txt`

`sender_myId<i>_otherId<j>_uirotk.txt`

### Help command

By running the help command (`-h` or `--help`) we can get some more information about the `simulator.py` command:


```
$ python3 simulator.py -h

>> usage: simulator.py [-h] [-s] [-n] [-t] [-p]
>>
>> Generate oblivious keys for oblivious transfer.
>>
>> optional arguments:
>>  -h, --help            show this help message and exit
>>  -s, --size-key    Insert the size of the oblivious key desired.
>>  -n, --num-key     Insert the number of oblivious keys desired in each
>>                        file.
>>  -t, --type-key    Insert the key type (integer) according to the
>>                        following encoding: 0 = unsint_rotkeys | 1 =
>>                        bin_rotkeys | 2 = unsint_okeys | 3 : bin_okeys.
>>  -p, --num-parties Insert the number of parties participating in the
>>                        protocol.
```