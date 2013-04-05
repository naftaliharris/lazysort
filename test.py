"""test.py"""

import unittest
import random
import lazysorted
import doctest
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
        """Selection should work once"""
        for n in xrange(1, 64):
            xs = range(n)
            for k in xrange(1, n):
                for rep in xrange(10):
                    random.shuffle(xs)
                    self.assertEqual(LazySorted(xs)[k], k,
                                     msg="xs = %s; k = %d" % (xs, k))

    def test_multiple_select(self):
        """Selection should work many times in a row"""
        for n in xrange(1, 64):
            xs = range(n)
            ks = 2 * range(n)  # include multiple accesses
            for rep in xrange(10):
                random.shuffle(xs)
                random.shuffle(ks)
                ls = LazySorted(xs)
                for k in ks:
                    self.assertEqual(ls[k], k, msg="xs = %s; ks = %s; k = %d" %
                                     (xs, ks, k))

    def test_len(self):
        """the __len__ method and len(.) builtin should work"""
        for n in xrange(1024):
            xs = range(n)
            ls = LazySorted(xs)
            self.assertEqual(len(ls), n)
            self.assertEqual(ls.__len__(), n)

    def test_select_range(self):
        """selecting contiguous foward ranges should work"""
        for n in xrange(128):
            xs = range(n)
            for list_rep in xrange(5):
                random.shuffle(xs)
                ls = LazySorted(xs)
                for select_rep in xrange(128):
                    a, b = random.randrange(n + 1), random.randrange(n + 1)
                    a, b = min(a, b), max(a, b)
                    self.assertEqual(ls[a:b], range(a, b), msg="xs = %s; "
                                     "(a, b) = (%d, %d); select_rep = %d" %
                                     (xs, a, b, select_rep))

    def test_full_range(self):
        """selecting slice objects with steps should work"""
        for n in xrange(128):
            xs = range(n)
            ys = range(n)
            for list_rep in xrange(5):
                random.shuffle(xs)
                ls = LazySorted(xs)
                for select_rep in xrange(128):
                    a = random.randrange(-n, n + 1)
                    b = random.randrange(-n, n + 1)
                    c = random.randrange(1, n + 3) * random.choice([-1, 1])
                    self.assertEqual(ls[a:b:c], ys[a:b:c], msg="xs = %s; "
                                     "called ls[%d:%d:%d]" % (xs, a, b, c))

    def test_step(self):
        """selecting slice objects with only a step defined should work"""
        steps = [-64, -16, -2, -1, 1, 2, 16, 64]
        for n in xrange(128):
            xs = range(n)
            ys = range(n)
            for list_rep in xrange(5):
                random.shuffle(xs)
                ls = LazySorted(xs)
                random.shuffle(steps)
                for step in steps:
                    self.assertEqual(ls[::step], ys[::step])

    def test_between(self):
        """the between method should work"""
        for n in xrange(128):
            xs = range(n)
            ys = range(n)
            for rep in xrange(100):
                a = random.randrange(-n, n + 1)
                b = random.randrange(-n, n + 1)

                random.shuffle(xs)
                ls = LazySorted(xs)
                between = ls.between(a, b)

                self.assertEqual(len(between), len(ys[a:b]), msg="n = %d; "
                                 "called ls.between(%d, %d)" % (n, a, b))
                self.assertEqual(set(between), set(ys[a:b]), msg="n = %d; "
                                 "called ls.between(%d, %d)" % (n, a, b))

    def test_README(self):
        """the examples in the README should all be correct"""
        failures, tests = doctest.testfile('README.md')
        self.assertEqual(failures, 0)

    def test_contains(self):
        """The __contains__ method and `in' keyword should work"""
        for n in xrange(128):
            xs = range(n)
            ys = range(0, n, 5) + [-4, -3, -2, -1, 0, n, n + 1, n + 2, 3.3]
            for rep in xrange(10):
                random.shuffle(xs)
                random.shuffle(ys)

                ls = LazySorted(xs)
                for y in ys:
                    self.assertEqual(y in xs, y in ls, msg="ys = %s; xs = %s" %
                                     (ys, xs))

                ls = LazySorted(xs)
                for y in ys:
                    self.assertEqual(xs.__contains__(y), ls.__contains__(y),
                                     msg="ys = %s; xs = %s" % (ys, xs))

    def test_simple_index(self):
        """The index method should work"""
        for n in xrange(128):
            xs = range(n)
            ys = range(n)
            for rep in xrange(5):
                random.shuffle(xs)
                random.shuffle(ys)
                ls = LazySorted(xs)

                for y in ys:
                    self.assertEqual(ls.index(y), y)

    def test_index_valueerror(self):
        """The index method should raise a ValueError if item not in list"""
        for n in xrange(128):
            xs = range(n)
            for rep in xrange(5):
                random.shuffle(xs)
                ls = LazySorted(xs)

                with self.assertRaises(ValueError):
                    ls.index(-1)

                with self.assertRaises(ValueError):
                    ls.index(n)

                with self.assertRaises(ValueError):
                    ls.index(5.5)

    def test_index_nonunique(self):
        """The index method should work in the presence of nonunique items"""
        for a in xrange(1, 32):
            for b in xrange(1, 32):
                xs = a * ["a"] + b * ["b"]
                for rep in xrange(3):
                    random.shuffle(xs)
                    ls = LazySorted(xs)

                    self.assertEquals(ls.index("b"), a)
                    self.assertEquals(ls.index("a"), 0)

                for rep in xrange(3):
                    random.shuffle(xs)
                    ls = LazySorted(xs)

                    self.assertEquals(ls.index("a"), 0)
                    self.assertEquals(ls.index("b"), a)

    def test_count_nonunique(self):
        """The count method should work in the presence of nonunique items"""
        for a in xrange(1, 32):
            for b in xrange(1, 32):
                xs = a * ["a"] + b * ["b"]
                for rep in xrange(3):
                    random.shuffle(xs)
                    ls = LazySorted(xs)

                    self.assertEquals(ls.count("b"), b)
                    self.assertEquals(ls.count("a"), a)

                for rep in xrange(3):
                    random.shuffle(xs)
                    ls = LazySorted(xs)

                    self.assertEquals(ls.count("a"), a)
                    self.assertEquals(ls.count("b"), b)

    def test_count_simple(self):
        """The count method should work on simple queries"""
        for n in xrange(128):
            xs = range(n)
            ys = range(0, n, 5) + [-4, -3, -2, -1, 0, n, n + 1, n + 2, 3.3]
            for rep in xrange(5):
                random.shuffle(xs)
                random.shuffle(ys)
                ls = LazySorted(xs)
                for y in ys:
                    self.assertEqual(ls.count(y), 1 if (isinstance(y, int) and
                                     0 <= y < n) else 0)

    def test_count_manynonunique(self):
        """The count method should work with very many nonunique items"""
        for rep in xrange(5000):
            items = range(random.randint(1, 50))
            random.shuffle(items)
            itemcounts = [random.randint(0, 16) for _ in items]
            xs = [y for x in [[i] * itemcounts[i] for i in items] for y in x]

            ls = LazySorted(xs)
            for item in items:
                self.assertEquals(ls.count(item), itemcounts[item])

        for n in xrange(1, 128):
            ls = LazySorted([0] * n)
            self.assertEquals(ls.count(0), n)


if __name__ == "__main__":
    unittest.main(verbosity=1)
