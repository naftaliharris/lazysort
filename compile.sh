#!/bin/bash
# Script for testing compilation on different python versions

rm -rf build

echo -e "\n\nPYTHON 2.5\n==========\n"
python2.5 setup.py build
read -p "Enter to continue"

echo -e "\n\nPYTHON 2.6\n==========\n"
python2.6 setup.py build
read -p "Enter to continue"

echo -e "\n\nPYTHON 2.7\n==========\n"
python2.7 setup.py build
read -p "Enter to continue"

echo -e "\n\nPYTHON 3.1\n==========\n"
python3.1 setup.py build
read -p "Enter to continue"

echo -e "\n\nPYTHON 3.2\n==========\n"
python3.2 setup.py build
read -p "Enter to continue"

echo -e "\n\nPYTHON 3.3\n==========\n"
python3.3 setup.py build
