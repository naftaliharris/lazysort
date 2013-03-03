"""
lazysorted.py
Author: Naftali Harris
License: BSD
"""


SORT_THRESH = 5


class LazySorted(object):

    def __init__(self, iterable, key=lambda x: x, reverse=False):
        """Note: No cmp, which is removed in python 3 anyway"""
        self.key = key
        self.reverse = reverse

        self._xs = list(iterable)

        # pivots are of the form (int, bool), with int the index and bool
        # whether or not the data is sorted over the increment ending at pivot
        # For every pivot i, we have the following invariants
        # 1) self._xs[i] == sorted(self._xs)[i]
        # 2) all(self._xs[j] < self.xs[i] for i in xrange(i))
        # 3) The increments between pivots are at least two
        # 4) The pivots are sorted
        self._pivots = []

    def _partition(self, start, end):
        """Picks a partition point in xs[start:end], and partially sorts
        xs[start:end] so that xs[start:end] is
        [points < part_point] + [points >= part_point].

        Then returns the index of the part_point."""

        if end - start <= SORT_THRESH:
            self._xs[start:end] = sorted(self._xs[start:end])

        else:
            part_point = self._xs[start]
            last_less = start
            for i in xrange(start + 1, end):
                if self._xs[i] < part_point:
                    last_less += 1
                    xs[last_less], xs[i] = xs[i], xs[last_less]
            xs[start], xs[last_less] = xs[last_less], xs[start]

            return last_less

    def __len__(self):
        return len(self._xs)

    def __getitem__(self, key):

        # Do the same type checks as in lists
        if isinstance(key, (int, long)):
            return self[key:key + 1][0]

        elif isinstance(key, slice):
            values = [None, None, None]
            for i, part in enumerate(["start", "stop", "step"]):
                value = key.__getattribute(part)
                if isinstance(value, (int, long, None)):
                    values[i] = value
                else:
                    try:
                        values[i] = value.__index__()
                    except AttributeError:
                        raise TypeError

            start, stop, step = values
            start = start or 0
            stop = stop = stop or len(self)

            if step == 0:
                raise ValueError("slice step cannot be zero")
            step = step or 1

            # actually get the results
            pass

        else:
            typename = str(type(key)).split("'")[1]
            raise TypeError("list indices must be integers, not %s" % typename)

    def __iter__(self):
        pass

    def __contains__(self, item):
        try:
            self.index(item)
            return True
        except ValueError:
            return False

    def index(self, item):
        """Returns the index of the first occurence of item, or raises a
        ValueError if the item is not in the list"""
        pass

    def count(self, item):
        try:
            i = self.index(item)
        except ValueError:
            return 0
        else:
            res = 1
            i += 1
            while i < len(self):
                if self[i] == item:
                    res += 1
            return res
