lazysorted
==========

lazysorted is a Python extension module for sorting sequences lazily. It
presents the programmer with the abstraction that they are actually working
with a sorted list, when in fact the list is only physically sorted when the
programmer requests elements from it, and even then it is only sorted partially,
just enough to return whatever was requested.

The LazySorted object has a constructor that implements the same interface as
the builtin `sorted(...)` function, and it supports most of the non-mutating
methods of a python list.

Since the LazySorted object only sorts as much as necessary, it can be faster
than using the builtin `sorted(...)` for tasks that do not require the entire
data to be sorted, like:

1.  Computing medians
2.  Computing [truncated means](http://en.wikipedia.org/wiki/Truncated%5Fmean)
3.  Quickly iterating through the first few sorted elements of a list
4.  Computing the deciles or quartiles of some data


How to use it
-------------

You can use LazySorted in much the same way you use the `sorted(...)` function
and the python lists it produces:

```python
from lazysorted import LazySorted
from math import floor, ceil


def median(xs):
    """An expected linear time median function"""
    ls = LazySorted(xs)
    n = len(ls)
    if n == 0:
        raise ValueError("Need a non-empty iterable")
    elif n % 2 == 1:
        return ls[n//2]
    else:
        return sum(ls[(n/2-1):(n/2+1)]) / 2.0


def top_k(xs, k, key=None, reverse=False):
    """Efficiently computes the top k elements of xs using the given key, or
    the bottom k if reverse=True"""
    ls = LazySorted(xs, key=key, reverse=reverse)
    return ls[0:k]
    

def trimmed_mean(xs, alpha=0.05):
    """Computes the mean of the xs from the alpha to (1-alpha) quantiles
    in expected linear time. More robust than the ordinary sample mean."""
    if not 0 <= alpha < 0.5:
        raise ValueError("alpha must be in [0, 0.5)")

    ls = LazySorted(xs)
    n = len(ls)
    if n == 0:
        raise ValueError("Need a non-empty iterable")
    lower = int(floor(n * alpha))
    upper = int(ceil(n * (1 - alpha)))
    return sum(ls.between(lower, upper)) / (upper - lower)

```

In addition to the `__len__` and `__getitem__` methods demostrated above,
LazySorted also supports the `__iter__`, `__contains__`, `index`, and `count`
methods, just like a regular python list:

```python
>>> import random
>>> from lazysorted import LazySorted
>>> xs = list(range(1000)) + 5 * [1234]
>>> random.shuffle(xs)
>>> ls = LazySorted(xs)
>>> for x in ls:
...     print(x)
...     if x >= 3:
...         break
0
1
2
3
>>> 1235 in ls
False
>>> ls.index(821)
821
>>> ls.count(1234)
5

```

Although the LazySorted constructor pretends to be equivalent to the `sorted`
function, and the LazySorted object pretends to be equivalent to a sorted python
list, there are a few differences between them:

1.  LazySorted objects are immutable, while python lists are not.
2.  Sorting with the builtin `sorted` function is guaranteed to be stable, (ie,
    preserve the original order of elements that compare equal), while
    LazySorted sorting is not stable.
3.  The LazySorted object has a `between(i, j)` method, which returns a list of
    all the items whose sorted indices are in `range(i, j)`, but not necessarily
    in order. This is useful, for example, for throwing away outliers when
    computing an alpha-trimmed mean.

When the APIs differ between python2.x and python3.x, lazysorted implements the
python3.x version. So the LazySorted constructor does not support the `cmp`
argument that was removed in python3.x, and the LazySorted object does not
support the `__getslice__` method that was also removed in python3.x.

All of the LazySorted methods have pretty good documentation, which can be
accessed through the builtin `help(...)` function.

I've tested lazysorted and found it to work for CPython versions 2.5, 2.6, 2.7,
and 3.1, 3.2, and 3.3. I haven't tested 3.0.


How it works
------------

In short, LazySorted works by using quicksort partitions lazily and keeping
track of the indices used as pivots.

**[quicksort](http://en.wikipedia.org/wiki/Quicksort)** sorts a list by picking
an element of the list to be the "pivot", and then partitioning the data into
the part that's greater than or equal to the pivot and the part that's less
than the pivot. These two parts are then recursively sorted with quicksort, 

**[quickselect](http://en.wikipedia.org/wiki/Quickselect)** finds the kth
smallest element of a list by picking a pivot element and partitioning the
data, as in quicksort. Then the algorithm recurses into the larger or smaller
part of the list, depending on whether k is larger or smaller than the index of
the pivot element.

There are two key observations to make from these algorithms: First of all, if
we are only interested in part of a sorted list, we only need to recurse into
the part we are interested in after doing a partition. Second of all, after
doing some partitions, the list is partially sorted, with the pivots all in
their sorted order and the elements between two pivots guaranteed to be bigger
than the pivot to their left and smaller than the pivot to their right.

So whenever some data is queried from a LazySorted object, we first look
through the pivots to see which pivots delimit the data we want. Then we
partition sublist(s) as necessary and recurse into the side(s) that our data is
in.

There are also some implementation details that help lazysorted to run quickly:
First of all, pivots elements are chosen to be the median of three randomly selected
elements, which makes the partition likely to be more balanced and guarantees
average case O(n log n) behavior.

Second of all, for sufficiently small lists, lazysorted uses insertion sort
instead of quicksort, which is faster on small lists. Both of these tricks are
well-known to speed up quicksort implementations.

Thirdly, since it's important to find the pivots that bound an index quickly,
lazysorted stores the pivots in a binary search tree, so that these sorts of
lookups occur in O(log n) expected time. The BST lazysorted uses is a
[Treap](http://en.wikipedia.org/wiki/Treap), selected for its overall expected
speed, especially in insertion and deletion.

lazysorted also makes a big effort to delete irrelevant pivots from the BST;
for example, if there are three pivots at indices 5, 26, and 42, and both the
data (between 5 and 26) and (between 26 and 42) is sorted, then we can remove
the irrelevant pivot 26, and just say that the data between indices 5 and 42 is
sorted.


Installation
------------

lazysorted requires the python headers, (Python.h). I believe they ship with
OSX, but if you don't have them they can be installed on a debian-like system
with
    
    $ sudo apt-get install python-dev

Then you can install lazysorted with

    $ sudo python setup.py install

Alternatively, you can install lazysorted from pypi with

    $ easy_install --user lazysorted

or
    
    $ pip install lazysorted

though you'll still need the python headers for it to build properly.


Testing
-------

I've put in a fair bit of effort to test that lazysorted actually does what
it's supposed to. You can test it yourself (after installing it) with

    $ python test.py


FAQ
---

**Doesn't numpy have a median and percentile function?**

Yes, but it's implemented by sorting the entire array and then reading off the
requested values, not with quickselect or another O(n) selection algorithm.
And LazySorted is empirically faster, as you can see from benchmark.py

**Isn't python3.4 going to have a statistics module with a median function?**

Yes, and I'm really excited about it! This is
[PEP450](http://www.python.org/dev/peps/pep-0450/). Unfortunately, the current
implementation is in pure python, and computes the median by sorting the data
and picking off the middle element.

**Doesn't the standard library have a heapq module?**

Yes, but it lacks the full generality of this module. For example, you can use
it to get the k smallest elements in O(n log k) time, but not k arbitrary
contiguous elements. This module represents a different paradigm: you're
allowed to program as if your list was sorted, and let the data structure deal
with the details.

**How is lazysorted licensed?**

lazysorted is BSD-licensed. So you can use it pretty much however you like!
See LICENSE for details.

**What should I not use lazysorted for?**

1.  Applications requiring a stable sort; the quicksort partitions make the
    order of equal elements in the sorted list undefined.
2.  Applications requiring guaranteed fast worst-case performance. Although
    it's very unlikely, many operations in LazySorted run in worst case O(n^2)
    time.
3.  Applications requiring high security. The random number generator is
    insecure and seeded from system time, so an (ambitious) attacker could
    reverse engineer the random number generator and feed LazySorted
    pathological lists that make it run in O(n^2) time.
4.  Sorting entire lists: The builtin `sorted(...)` is *very* impressively
    designed and implemented. It also has the advantage of running faster than
    O(n log n) on lists with partial structure.

**How does lazysorted work at scale?**

Unfortunately, only okay. This turns out to be primarily due to the fact that
CPython deals with python objects by passing around pointers to them, causing
cache misses when the list and its elements no longer fit in cache. The gory
details can be found in a blog post I wrote about [Memory Locality and Python
Objects](http://www.naftaliharris.com/blog/heapobjects).

However, this effect doesn't kick in until lists grow larger than about 100K
values, and even past that lazysorted remains faster than complete sorting.


Contact me!
-----------

If you use this software and feel so inclined, I'd greatly appreciate hearing
what you are using it for! You can hit me up on Twitter
[@naftaliharris](https://twitter.com/naftaliharris), or at my email address
on my [contact page](http://www.naftaliharris.com/contact/).
