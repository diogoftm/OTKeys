import random



def gen_rotkeys(keysize, numkey):

    alice_key_list = []
    bob_key_list = []
    for ks in range(keysize):
        alice_key_list.append(random.randint(0,1))
        



    for i in alice_key_list:
        b = random.randint(0,1) # bit known by Bob: 0 is the first of the block, 1 is the second of the block
        alice_key_list.append(random.randint(0,1))

    return alice_key_list


def main(keysize, numkey, typekey=0):

    if keysize % 2 != 0:
        raise ValueError("The argument ::keysize:: must be even")

    otkeys = 0

    if typekey == otkeys:
        return gen_rotkeys(keysize, numkey)




if __name__ == "__main__":
    print(main(13, 1))