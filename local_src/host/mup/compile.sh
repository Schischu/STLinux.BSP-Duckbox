#!/bin/bash

if [  -e mup ]; then
  rm mup
fi

if [  -e mup.exe ]; then
  rm mup.exe
fi

g++ -o mup mup.cpp misc.cpp crc32.cpp sh1.cpp swinventory.cpp swpack.cpp swunity.cpp
