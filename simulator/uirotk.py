import random
from typing import Union
from aux import header, generate_pairs

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




def uns_int_rotk(keysize : int, numkey : int, numpart : int) -> None:

    for my_num, other_player in generate_pairs(numpart):
        
        ## generate keys
        sender_keys, receiver_keys = gen_rotkeys_list(keysize, numkey)
        
        ## Generate sender's file
        sender_path_to_ok = "../keys/sender_myId" + str(my_num) + "_otherId" + str(other_player) + "_uirotk.txt"
        f_sender = open(sender_path_to_ok, "w")
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
        ## Generate sender's file
        receiver_path_to_ok = "../keys/receiver_myId" + str(my_num) + "_otherId" + str(other_player) + "_uirotk.txt"
        f_receiver = open(receiver_path_to_ok, "w")
        # print header
        h_receiver = header("receiver", keysize, numkey)
        f_receiver.write(h_receiver)
        # print all sender's keys
        for rotk in receiver_keys:
            rotk_str = ''.join(str(elm) for elm in rotk)
            f_receiver.write("\n")
            f_receiver.write(rotk_str)
        f_receiver.close()