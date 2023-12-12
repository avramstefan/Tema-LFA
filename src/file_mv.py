#!/usr/bin/env python3
import sys
import os

if __name__ == "__main__":
    argv = sys.argv[1:]
    os.system(f"cp ../teste/test{argv[0]}.txt input.txt")

