#!/usr/bin/python

import sys
import math

# Command line arguments
n           = float(sys.argv[1])
alphSize    = float(sys.argv[2])
INT         = float(sys.argv[3])
alpha        = float(sys.argv[4])

# Constants
l           = math.log(n, 2) * alpha

# Calculation

def toMB(byteSize):
    return byteSize / float(1024  * 1024)

def printOccMB(name, byteSize):
    print "%s size:\t %8.3fM" % (name, toMB(byteSize))


alphOcc = alphSize
MTFStatesOcc = n * alphSize / l

sbNOOcc = n / math.pow(l, 2) * alphSize * INT
sbWOcc = n / math.pow(l, 2) * INT

bNOOcc = n / l * alphSize * INT
bWOcc = n / l * INT

ZOcc = n

totalOccBytes = alphOcc + MTFStatesOcc + \
                sbNOOcc + sbWOcc + \
                bNOOcc + bWOcc + \
                ZOcc

totalOccMB = toMB(totalOccBytes)

# Output

printOccMB("Alphabet", alphOcc)
printOccMB("MTFStates", MTFStatesOcc)
print "----------------------------------"
printOccMB("sbNO", sbNOOcc)
printOccMB("sbW", sbWOcc)
print "----------------------------------"
printOccMB("bNO", bNOOcc)
printOccMB("bW", bWOcc)
print "----------------------------------"
printOccMB("Z\t", ZOcc);
print "----------------------------------"
print "Total:\t\t %8.3fM" % totalOccMB
