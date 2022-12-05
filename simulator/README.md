## Oblivious key simulator

The idea of the simulator is simply to make the test easier and smoother. The simulator supports that following OT types:

1. Random oblivious transfer


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