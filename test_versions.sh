#!/bin/bash
# Script for testing on different python versions

pandoc --from=markdown --to=rst --output=README.txt README.md

function restore {
    if [ -f test.py.bak ]
    then
        mv test.py.bak test.py
    fi
}

function check_success {
    if [ $? -ne 0 ]
    then
        restore
        exit 1
    fi
}

rm -rf build

for version in 2.7-dbg 2.5 2.6 2.7 3.1 3.2 3.3
do
echo -e "\n\nPYTHON $version\n==========\n"
CFLAGS="-UNDEBUG" python$version setup.py build
check_success
echo ""

loc=$version
if [ "$version" == "2.7-dbg" ]
then
    loc="2.7-pydebug"
fi
if [ "${version%.*}" == "3" ]
then
    if [ ! -f test.py.bak ]
    then
        2to3 --no-diffs -w test.py
    fi
fi

PYTHONPATH="build/lib.linux-x86_64-$loc/" python$version -c "import lazysorted; print(lazysorted);"
PYTHONPATH="build/lib.linux-x86_64-$loc/" python$version test.py
check_success
done

restore
