"""
benchmark.py
Author: Naftali Harris
License: BSD
"""

from __future__ import division
import random
from timeit import repeat
from lazysorted import LazySorted


def random_median():
    n = 1000001
    xs = range(n)
    reshuffle = lambda: random.shuffle(xs)
    full_median = lambda: sorted(xs)[n // 2]
    lazy_median = lambda: LazySorted(xs)[n // 2]

    full_data = repeat(full_median, setup=reshuffle, repeat=100, number=1)
    lazy_data = repeat(lazy_median, setup=reshuffle, repeat=100, number=1)

    print sum(full_data) / len(full_data)
    print sum(lazy_data) / len(lazy_data)


def main():
    random_median()

if __name__ == "__main__":
    main()
