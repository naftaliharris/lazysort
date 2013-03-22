"""test.py"""

import unittest
import random
import lazysorted
from lazysorted import LazySorted


class TestLazySorted(unittest.TestCase):

    def test_creation(self):
        """LazySorted objects can be created from any iterable"""
        x = LazySorted([])
        x = LazySorted([1, 2, 3, 4])
        x = LazySorted(x for x in range(100) if x % 3 == 0)
        x = LazySorted((3, -2, 5))
        x = LazySorted(xrange(100))
        x = LazySorted(xrange(0))
        x = LazySorted({"foo": 10, "bar": 3, "baz": 9})

    def test_random_select(self):
        for n in xrange(1, 64):
            xs = range(n)
            for k in xrange(1, n):
                for rep in xrange(1, 10):
                    random.shuffle(xs)
                    self.assertEqual(LazySorted(xs)[k], k,
                                     msg="xs = %s; k = %d" % (xs, k))

    def test_multiple_select(self):
        for n in xrange(1, 64):
            xs = range(n)
            ks = 2 * range(n)  # include multiple accesses
            for rep in xrange(1, 10):
                random.shuffle(xs)
                random.shuffle(ks)
                ls = LazySorted(xs)
                for k in ks:
                    self.assertEqual(ls[k], k, msg="xs = %s; ks = %s; k = %d" %
                                     (xs, ks, k))

    def test_len(self):
        for n in xrange(1024):
            xs = range(n)
            ls = LazySorted(xs)
            self.assertEqual(len(ls), n)
            self.assertEqual(ls.__len__(), n)

    def test_select_range(self):
        for n in xrange(1, 128):
            xs = range(n)
            for list_rep in xrange(1, 5):
                random.shuffle(xs)
                ls = LazySorted(xs)
                for select_rep in xrange(128):
                    a, b = random.randrange(n), random.randrange(n)
                    a, b = min(a, b), max(a, b)
                    self.assertEqual(ls[a:b], range(a, b), msg="xs = %s; "
                                     "(a, b) = (%d, %d); select_rep = %d" %
                                     (xs, a, b, select_rep))

if __name__ == "__main__":
    unittest.main()
