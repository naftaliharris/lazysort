lazysorted
==========

lazysorted is a Python extension module for sorting sequences lazily. It
implements the same interface as the builtin sorted(.) function. Since it only
sorts as much as necessary, it can be faster than using the builtin sorted(.)
for tasks that do not require the entire data to be sorted, like:

1. Computing medians
2. Computing (truncated means)[http://en.wikipedia.org/wiki/Truncated%5Fmean]
3. Quickly iterating through the first few sorted elements of a list
4. Computing the deciles or quartiles of some data

How to use it
-------------

You can use LazySorted much the same way you use python lists and the sorted(.)
function:

```python
>>> import random
>>> from lazysorted import LazySorted
>>> xs = list(range(1000))
>>> random.shuffle(xs)
>>> xs_sorted = LazySorted(xs)
>>> xs_sorted[43]
43
>>> xs_sorted[497:503]
[497, 498, 499, 500, 501, 502]
>>> -1 in xs_sorted
False
>>> xs_sorted.index(821)
821
>>> xs_sorted.count(52)
1

```

LazySorted objects have a few extra methods, however: (unsorted data between
intervals for alpha-trimmed mean, for example)

All of these methods have pretty good documentation, which can be accessed
through the builtin help(.) function.

How it works
------------

In short, LazySorted works by using quicksort partitions lazily and keeping
track of the indices used as pivots.

**quicksort** sorts a list by picking an element of the list to be the "pivot",
and then partitioning the data into the part that's greater than or equal to
the pivot and the part that's less than the pivot. These two parts are then
recursively sorted with quicksort. (http://en.wikipedia.org/wiki/Quicksort)

**quickselect** finds the kth smallest element of a list by picking a pivot
element and partitioning the data, as in quicksort. Then the algorithm recurses
into the larger or smaller part of the list, depending on whether k is larger
or smaller than the index of the pivot element.
(http://en.wikipedia.org/wiki/Quickselect)

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
First of all, pivots elements are chosen to be the median of three randomly
elements, which makes the partition likely to be more balanced and guarantees
average case nlog(n) behavior. Second of all, for sufficiently small lists, we
use insertion sort instead of quicksort, which is faster on small lists. Both
of these tricks are well-known to speed up quicksort implementations. Thirdly,
since it's important to find the pivots that bound an index quickly, we also
store the pivots in a binary search tree, so that these sorts of lookups occur
in log(n) expected time. The BST we use is a Treap, selected for its overall
expected speed, especially in insertion and deletion,
(http://en.wikipedia.org/wiki/Treap). We also make a big effort to delete
irrelevant pivots from the BST; for example, if there are three pivots at
indices 5, 26, and 42, and both the data (between 5 and 26) and (between 26 and
42) is sorted, then we can remove the irrelevant pivot 26, and just say that
the data between indices 5 and 42 is sorted.


Installation
------------

lazysorted requires the python headers, (Python.h). I believe they ship with
OSX, but if you don't have them they can be installed on a debian-like system
with
    
    $ sudo apt-get install python-dev

Then you can install lazysorted with

    $ sudo python setup.py install


Testing and Performance
-----------------------

I've put in a fair bit of effort to test that lazysorted actually does what
it's supposed to. You can test it yourself (after installing it) with

    $ python test.py

Blah blah benchmarks


Example
-------

```python
def median(xs):
    """An expected linear time median function"""
    n = len(xs)
    if n == 0:
        raise ValueError("Need a non-empty list")
    elif n % 2 == 1:
        return LazySorted(xs)[n//2]
    else:
        return sum(LazySorted(xs)[(n/2-1):(n/2+1)]) / 2.0

```

FAQ
---

**Doesn't numpy have a median and percentile function?**

Yes, but it's implemented by sorting the entire array and then reading off the
requested values, not with quickselect or another O(n) selection algorithm.
And LazySorted is empirically faster, as you can see from benchmark.py

**Doesn't the standard library have a heapq module?**

Yes, but it lacks the full generality of this module. For example, you can use
it to get the k smallest elements, (in n * log(k) time), but not k arbitrary
contiguous elements. This module represents a different paradigm: you're
allowed to program as if your list was sorted, and let the module deal with the
details.

**How is lazysorted licensed?**

lazysorted is BSD-licensed. So you can use it pretty much however you like!
See LICENSE for details.

**What should I not use lazysorted for?**

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
   and implemented. It also has the advantage of running faster than nlog(n)
   on lists with partial structure.

**How does lazysorted work at scale?**

Unfortunately, only okay. This turns out to be basically due to the fact that
CPython deals with python objects by passing around pointers to them. The gory
details can be found on (my blog)[http://www.naftaliharris.com/blog/heapobjects].

**What python versions does lazysorted work with?**

I've tried out 2.5, 2.6, 2.7, 3.1, 3.2, and 3.3, and it works with all of those.

Contact me!
---------

If you use this software and feel so inclined, I'd greatly appreciate hearing
just that you tried it out. My email address can be found by running this code:

```python
first_name = "naftali"
last_name = "harris"
domain = "gmail.com"

print first_name + last_name + '@' + domain
```

Even something as simple as the following would be mega-awesome:

    Hey Naftali,
    
    I tried out lazysorted to compute medians faster. Thought you'd like to know!
    
    --Jim

There is of course no obligation to do this.
