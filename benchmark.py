"""
benchmark.py
Author: Naftali Harris
License: BSD
"""

from __future__ import division
import random
from timeit import repeat
import numpy as np
from lazysorted import LazySorted


def random_median():
    n = 10001
    trials = 100
    xs = range(n)
    reshuffle = lambda: random.shuffle(xs)
    full_median = lambda: sorted(xs)[n // 2]
    numpy_median = lambda: np.median(xs)
    lazy_median = lambda: LazySorted(xs)[n // 2]

    full_data = repeat(full_median, setup=reshuffle, repeat=trials, number=1)
    numpy_data = repeat(numpy_median, setup=reshuffle, repeat=trials, number=1)
    lazy_data = repeat(lazy_median, setup=reshuffle, repeat=trials, number=1)

    print "Full time / LazySorted Time:"
    print sum(full_data) / sum(lazy_data)
    print "Numpy time / LazySorted Time:"
    print sum(numpy_data) / sum(lazy_data)


def main():
    random_median()

if __name__ == "__main__":
    main()
