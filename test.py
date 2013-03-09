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
                    self.assertEqual(LazySorted(xs)[k], k)


if __name__ == "__main__":
    unittest.main()
