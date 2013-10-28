#!/bin/bash
# Script for testing compilation on different python versions

rm -rf build

for version in 2.5 2.6 2.7
do
echo -e "\n\nPYTHON $version\n==========\n"
python$version setup.py build
PYTHONPATH="build/lib.linux-x86_64-$version/" python$version test.py
done

2to3 --no-diffs -w test.py

for version in 3.1 3.2 3.3
do
echo -e "\n\nPYTHON $version\n==========\n"
python$version setup.py build
PYTHONPATH="build/lib.linux-x86_64-$version/" python$version test.py
done

mv test.py.bak test.py
