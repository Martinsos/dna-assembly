#!/bin/bash
LEDA="-I../LEDA/incl -L../LEDA -lleda -lX11"
g++ ledaTest.cpp $LEDA
