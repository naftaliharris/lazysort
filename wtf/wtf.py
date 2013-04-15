"""
wtf.py
Date: 2013-04-05
Author: Naftali Harris
"""
from __future__ import division
import matplotlib.pyplot as plt
import numpy as np
import time
import random
from lazysorted import LazySorted


def plotwtf(algo, bigO):
    reps = 30
    powers = np.linspace(1, 7.5, 50)
    ns = [int(10 ** p) + 1 for p in powers]
    times = []

    for n in ns:
        print n
        xs = range(n)
        T = 0.0
        for rep in xrange(reps):
            random.shuffle(xs)
            t = time.time()
            algo(xs)
            T += (time.time() - t)
        times.append(T / reps)

    ns = np.array(ns)
    times = np.array(times)
    plt.xlabel("List size (items)")
    plt.ylabel("Compute time per item (secs / item)")
    plt.title("Time to Compute the Median")
    plt.xscale('log')
    plt.plot(ns, times / bigO(ns))
    plt.show()

    """
    plt.xscale('log')
    plt.yscale('log')
    plt.plot(ns, times)

    beta = np.polyfit(np.log(ns), np.log(times), 2)
    yhat = np.polyval(beta, np.log(ns))
    plt.plot(ns, np.exp(yhat))
    plt.show()
    print beta
    print np.var(yhat) / np.var(np.log(times))
    """

def profile():
    reps = 1000
    n = 10 ** 6 + 1
    median = n // 2 + 1
    xs = range(n)
    random.shuffle(xs)
    for rep in xrange(reps):
        ls = LazySorted(xs)
        ls[median]
        if rep % 10 == 0:
            print rep


if __name__ == "__main__":
    plotwtf(lambda xs: LazySorted(xs)[len(xs) // 2], lambda ns: ns)
    #plotwtf(lambda xs: sorted(xs)[len(xs) // 2], lambda ns: ns * np.log(ns))
