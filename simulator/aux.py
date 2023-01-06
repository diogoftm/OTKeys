from itertools import combinations

def generate_pairs(n):
    """
    Generate all pairs of numbers from the set {0, ..., n-1} without repeted entries (i, i).
    """
    for i in range(0, n):
        for j in range(0, n):
            if i != j:
                yield i, j



def header(role : str, keysize : int, numkey : int) -> str:
    
    header = """Role: """+role+"""
IPOtherParty: 192.168.0.5
SizeOKeys: """+str(keysize)+"""
NumberOKeys: """+str(numkey)

    return header