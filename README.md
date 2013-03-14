lazysorted
==========

lazysorted is a Python module for sorting iterables lazily. It implements an
interface like the builtin sorted(.) function.


How to use it
-------------

In short, you can use LazySorted the same way you use the sorted(.) function:

    >>> import random
    >>> from lazysorted import LazySorted
    >>> xs = range(1000)
    >>> random.shuffle(xs)
    >>> xs_sorted = LazySorted(xs)
    >>> xs_sorted[497:503]
    [497, 498, 499, 500, 501, 502]
    >>> xs_sorted[999]
    999
    >>> "foo" in xs_sorted
    False

LazySorted objects have a few extra methods, however: (unsorted data between
intervals for alpha-trimmed mean, for example)

How it works
------------

In short, LazySorted works by using quicksort partitions lazily and keeping
track of the indices used as pivots.


Things you can use lazysorted for
---------------------------------

1. Computing the median of some data
2. Computing the alpha-trimmed mean
3. Quickly iterating through the first few sorted elements of a list
4. Computing the deciles or quartiles of some distribution

All of these will be found in examples.py


Things you should not use lazysorted for
----------------------------------------

1. Applications requiring a stable sort; the quicksort partitions make the
   order of equal elements in the sorted list undefined.
2. Applications requiring guaranteed fast worst-case performance. Although
   it's very unlikely, all operations in LazySorted run in worst case O(n^2)
   time.
3. Applications requiring high security. The random number generator is
   insecure and seeded from system time, so an (ambitious) attacker could
   reverse engineer the random number generator and feed LazySorted
   pathological lists that make it run in O(n^2) time.
4. Sorting entire lists: The builtin sorted(.) is very impressively designed
   and implemented.


Installation
------------

You can install lazysorted with

    $ sudo python setup.py install


Testing
-------

I've put in a fair bit of effort to test that lazysorted actually does what
it's supposed to. You can test it yourself with

    $ python test.py


Performance
-----------

Insert benchmark comparisons to sorted


Performance Tuning
------------------

...
