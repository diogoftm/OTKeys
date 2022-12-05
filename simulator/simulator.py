import random
from typing import Union

######################## aux.py

def header(role : str, keysize : int, numkey : int) -> str:
    
    header = """Role: """+role+"""
IPOtherParty: 192.168.0.5
SizeOKeys: """+str(keysize)+"""
NumberOKeys: """+str(numkey)

    return header

######################## binary_ok.py

######################## unsigned_ok.py

######################## binary_rot.py

######################## unsigned_int_rot.py

def gen_rotkey_list(keysize : int) -> Union[list, list]:

    sender_key = []
    receiver_key = []

    # create sender oblivious key: random numbers
    for _ in range(keysize):
        sender_key.append(random.randint(0,1))
        
    # create receiver oblivious key: random elements by sender from each 2-bit block
    len_sender_key_list = len(sender_key)
    for i in range(int(len_sender_key_list/2)):
        # bit known by receiver: b=0 for the first element, b=1 for the second element
        b = random.randint(0,1) 
        receiver_key.append(b)
        # add the known element (m_b) to receiver's list
        m_b = sender_key[2*i + b]
        receiver_key.append(m_b)

    return sender_key, receiver_key


def gen_rotkeys_list(keysize : int, numkey : int) -> Union[list, list]:

    sender_keys = []
    receiver_keys = []

    for _ in range(numkey):
        sender_key, receiver_key = gen_rotkey_list(keysize)
        sender_keys.append(sender_key)
        receiver_keys.append(receiver_key)
    
    return sender_keys, receiver_keys



def uns_int_rotk(keysize : int, numkey : int) -> None:

    ## generate keys
    sender_keys, receiver_keys = gen_rotkeys_list(keysize, numkey)
    
    ## Generate sender's file
    f_sender = open("sender_rotk.txt", "w")
    # print header
    h_sender = header("sender", keysize, numkey)
    f_sender.write(h_sender)
    # print all sender's keys
    for rotk in sender_keys:
        rotk_str = ''.join(str(elm) for elm in rotk)
        f_sender.write("\n")
        f_sender.write(rotk_str)
    f_sender.close()

    ## Generate receiver's file
    f_receiver = open("receiver_rotk.txt", "w")
    # print header
    h_receiver = header("receiver", keysize, numkey)
    f_receiver.write(h_receiver)
    # print all sender's keys
    for rotk in receiver_keys:
        rotk_str = ''.join(str(elm) for elm in rotk)
        f_receiver.write("\n")
        f_receiver.write(rotk_str)
    f_receiver.close()



######################## simulator.py


def main(keysize : int, numkey : int, typekey : int=0) -> int:

    if keysize % 2 != 0:
        raise ValueError("The argument ::keysize:: must be even")

    uns_int_rotkeys = 0
    bin_rotkeys = 1
    uns_int_okeys = 2
    bin_okeys = 3

    if typekey == uns_int_rotkeys:
        print("Generating Random OT keys.")
        uns_int_rotk(keysize, numkey)
        print("Random OT keys generated and saved to this current folder.")
        return 1
    elif typekey == bin_rotkeys:
        print("Method not available. Coming soon.")
        return 0
    elif typekey == uns_int_okeys:
        print("Method not available. Coming soon.")
        return 0
    elif typekey == bin_okeys:
        print("Method not available. Coming soon.")
        return 0




if __name__ == "__main__":
    main(512, 2)