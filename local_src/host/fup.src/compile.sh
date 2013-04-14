#!/bin/bash

if [  -e fup ]; then
  rm fup
fi

if [  -e fup.exe ]; then
  rm fup.exe
fi

g++ -o fup fup.c crc16.c crc32.cpp -lz -D$1
