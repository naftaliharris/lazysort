"""test.py"""

import unittest
import random
from itertools import islice
import doctest
import lazysorted
from lazysorted import LazySorted


class TestLazySorted(unittest.TestCase):
    test_lengths = range(18) + [31, 32, 33, 63, 64, 65, 127, 128, 129]

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
        for n in TestLazySorted.test_lengths:
            xs = range(n)
            for k in xrange(1, n):
                for rep in xrange(10):
                    random.shuffle(xs)
                    self.assertEqual(LazySorted(xs)[k], k,
                                     msg="xs = %s; k = %d" % (xs, k))

    def test_multiple_select(self):
        """Selection should work many times in a row"""
        for n in TestLazySorted.test_lengths:
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
        for n in TestLazySorted.test_lengths:
            xs = range(n)
            ls = LazySorted(xs)
            self.assertEqual(len(ls), n)
            self.assertEqual(ls.__len__(), n)

    def test_select_range(self):
        """selecting contiguous forward ranges should work"""
        for n in TestLazySorted.test_lengths:
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
        for n in TestLazySorted.test_lengths:
            xs = range(n)
            ys = range(n)
            for list_rep in xrange(5):
                random.shuffle(xs)
                ls = LazySorted(xs)
                for select_rep in xrange(16):
                    a = random.randrange(-n, n + 1)
                    b = random.randrange(-n, n + 1)
                    c = random.randrange(1, n + 3) * random.choice([-1, 1])
                    self.assertEqual(ls[a:b:c], ys[a:b:c], msg="xs = %s; "
                                     "called ls[%d:%d:%d]" % (xs, a, b, c))

    def test_step(self):
        """selecting slice objects with only a step defined should work"""
        steps = [-64, -16, -2, -1, 1, 2, 16, 64]
        for n in TestLazySorted.test_lengths:
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
        for n in TestLazySorted.test_lengths:
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
        for n in TestLazySorted.test_lengths:
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
        for n in TestLazySorted.test_lengths:
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
        for n in TestLazySorted.test_lengths:
            xs = range(n)
            for rep in xrange(5):
                random.shuffle(xs)
                ls = LazySorted(xs)

                self.assertRaises(ValueError, lambda: ls.index(-1))
                self.assertRaises(ValueError, lambda: ls.index(n))
                self.assertRaises(ValueError, lambda: ls.index(5.5))

    def test_index_nonunique(self):
        """The index method should work in the presence of nonunique items"""
        for a in xrange(1, 32):
            for b in xrange(1, 32):
                xs = a * ["a"] + b * ["b"]
                for rep in xrange(3):
                    random.shuffle(xs)
                    ls = LazySorted(xs)

                    self.assertEqual(ls.index("b"), a)
                    self.assertEqual(ls.index("a"), 0)

                for rep in xrange(3):
                    random.shuffle(xs)
                    ls = LazySorted(xs)

                    self.assertEqual(ls.index("a"), 0)
                    self.assertEqual(ls.index("b"), a)

    def test_count_nonunique(self):
        """The count method should work in the presence of nonunique items"""
        for a in xrange(1, 32):
            for b in xrange(1, 32):
                xs = a * ["a"] + b * ["b"]
                for rep in xrange(3):
                    random.shuffle(xs)
                    ls = LazySorted(xs)

                    self.assertEqual(ls.count("b"), b)
                    self.assertEqual(ls.count("a"), a)

                for rep in xrange(3):
                    random.shuffle(xs)
                    ls = LazySorted(xs)

                    self.assertEqual(ls.count("a"), a)
                    self.assertEqual(ls.count("b"), b)

    def test_count_simple(self):
        """The count method should work on simple queries"""
        for n in TestLazySorted.test_lengths:
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
        for rep in xrange(2000):
            items = range(random.randint(1, 50))
            random.shuffle(items)
            itemcounts = [random.randint(0, 16) for _ in items]
            xs = [y for x in [[i] * itemcounts[i] for i in items] for y in x]

            ls = LazySorted(xs)
            for item in items:
                self.assertEqual(ls.count(item), itemcounts[item])

        for n in TestLazySorted.test_lengths:
            ls = LazySorted([0] * n)
            self.assertEqual(ls.count(0), n)

    def test_sorting(self):
        """Iteration should be equivalent to sorting"""
        for length in TestLazySorted.test_lengths:
            items = range(length)
            random.shuffle(items)
            self.assertEqual(list(LazySorted(items)), range(length))

    def test_interupted_iter(self):
        """Iteration should work even if it's interrupted by other calls"""
        for rep in xrange(100):
            items = range(512)
            random.shuffle(items)
            ls = LazySorted(items)
            it = iter(ls)
            self.assertEqual(list(islice(it, 30)), range(0, 30))
            _ = ls[random.randrange(512)]
            _ = random.randrange(-100, 600) in ls
            self.assertEqual(list(islice(it, 30)), range(30, 60))

    def test_reverse(self):
        """Reverse iteration should be equivalent to reverse sorting"""
        for length in TestLazySorted.test_lengths:
            items = range(length)
            random.shuffle(items)
            self.assertEqual(list(LazySorted(items, reverse=True)),
                             range(length-1, -1, -1))

    def test_keys(self):
        """Using keys should work fine, with or without reverse"""
        for rep in xrange(100):
            items = [(random.random(), random.random()) for _ in xrange(256)]
            random.shuffle(items)
            for reverse in [True, False]:
                self.assertEqual(list(LazySorted(items, key=lambda x: x[0])),
                                 sorted(items, key=lambda x: x[0]))
                self.assertEqual(list(LazySorted(items, key=lambda x: x[1])),
                                 sorted(items, key=lambda x: x[1]))

    def test_API(self):
        """The sorted(...) API should be implemented except for cmp"""
        xs = range(10)
        for tryme in [lambda: LazySorted(xs, reverse="foo"),
                      lambda: LazySorted(xs, key="foo"),
                      lambda: LazySorted(xs, reverse=True, key="foo"),
                      lambda: LazySorted(xs, key=5),
                      lambda: LazySorted(xs, reverse="foo", key=lambda x: x),
                      lambda: LazySorted(xs, reverse=True, key=5)]:
            self.assertRaises(TypeError, tryme)

        # NB: LazySorted(xs, reverse=1.5) will succeed in python2.6 and down,
        # even though it should really fail. This was fixed in python2.7 and
        # up. See issue 5080 for details: http://bugs.python.org/issue5080

        # Keyword order shouldn't matter if they're named, but should if not
        LazySorted(xs, key=lambda x: x, reverse=False)
        LazySorted(xs, reverse=False, key=lambda x: x)
        LazySorted(xs, lambda x: x, False)
        self.assertRaises(TypeError, lambda: LazySorted(xs, 0, lambda x: x))

        # You can't call LazySorted without arguments
        self.assertRaises(TypeError, lambda: LazySorted())


if __name__ == "__main__":
    unittest.main()
