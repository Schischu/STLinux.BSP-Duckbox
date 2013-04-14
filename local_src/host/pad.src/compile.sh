#!/bin/bash

if [  -e pad ]; then
  rm pad
fi

if [  -e pad.exe ]; then
  rm pad.exe
fi

g++ -o pad pad.c
