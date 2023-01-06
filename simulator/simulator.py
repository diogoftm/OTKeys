import argparse
from uirotk import uns_int_rotk


## Parser ##

def main_parser_args():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description="Generate oblivious keys for oblivious transfer.",
    )
    parser.add_argument(
        "-s",
        "--size-key",
        help="""Insert the size of the oblivious key desired.
        """,
        type=int,
        default=512,
        metavar="\b",
    )
    parser.add_argument(
        "-n",
        "--num-key",
        help="""Insert the number of oblivious keys desired in each file.
        """,
        type=int,
        default=128,
        metavar="\b",
    )
    parser.add_argument(
        "-t",
        "--type-key",
        help="""Insert the key type (integer) according to the following encoding:\n
        0 = unsint_rotkeys | 1 = bin_rotkeys | 2 = unsint_okeys | 3 : bin_okeys.
        """,
        type=int,
        default=0,
        metavar="\b",
    )
    parser.add_argument(
        "-p",
        "--num-parties",
        help="""Insert the number of parties participating in the protocol.
        """,
        type=int,
        default=2,
        metavar="\b",
    )

    return parser.parse_args()


## Main ##

def main() -> int:

    unsint_rotkeys = 0
    bin_rotkeys = 1
    unsint_okeys = 2
    bin_okeys = 3

    args = main_parser_args()

    keysize = args.size_key
    numkey = args.num_key
    typekey = args.type_key
    numpart = args.num_parties

    if keysize % 2 != 0:
        raise ValueError("The argument ::keysize:: must be even")

    if typekey == unsint_rotkeys:
        print("Generating Random OT keys.")
        uns_int_rotk(keysize, numkey, numpart)
        print("Random OT keys generated and saved to this current folder.")
        return 1
    elif typekey == bin_rotkeys:
        print("Method not available. Coming soon.")
        return 0
    elif typekey == unsint_okeys:
        print("Method not available. Coming soon.")
        return 0
    elif typekey == bin_okeys:
        print("Method not available. Coming soon.")
        return 0




if __name__ == "__main__":
    main()