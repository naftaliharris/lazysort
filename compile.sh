#!/bin/bash
# Script for testing compilation on different python versions

rm -rf build

for version in 2.5 2.6 2.7
do
echo -e "\n\nPYTHON $version\n==========\n"
python$version setup.py build
if [ $? -ne 0 ]
then
    exit 1
fi
echo ""
PYTHONPATH="build/lib.linux-x86_64-$version/" python$version -c "import lazysorted; print(lazysorted);"
PYTHONPATH="build/lib.linux-x86_64-$version/" python$version test.py
if [ $? -ne 0 ]
then
    exit 1
fi
done

2to3 --no-diffs -w test.py

for version in 3.1 3.2 3.3
do
echo -e "\n\nPYTHON $version\n==========\n"
python$version setup.py build
if [ $? -ne 0 ]
then
    mv test.py.bak test.py
    exit 1
fi
echo ""
PYTHONPATH="build/lib.linux-x86_64-$version/" python$version -c "import lazysorted; print(lazysorted);"
PYTHONPATH="build/lib.linux-x86_64-$version/" python$version test.py
if [ $? -ne 0 ]
then
    mv test.py.bak test.py
    exit 1
fi
done

mv test.py.bak test.py
