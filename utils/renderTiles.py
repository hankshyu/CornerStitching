'''
Program: renderTiles.py
January 20th, 2024
Iris Lab

A program to visiualize and interact with cornerStitching tiles distribution, an useful debugging tool
'''

import sys
sys.path.append("./lib/")
from argparse import ArgumentParser
import re

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import math
import time
from distinctColours import ColourGenerator
import random
from random import seed

from typing import Optional, Literal
import matplotlib as mpl
import matplotlib.patches as mpatches
import matplotlib.pyplot as plt
from matplotlib.text import Annotation
from matplotlib.transforms import Transform, Bbox
from matplotlib.widgets import CheckButtons
from matplotlib.widgets import Button

COLORRST    = "\u001b[0m"
BLACK       = "\u001b[30m"
RED         = "\u001b[31m"
GREEN       = "\u001b[32m"
YELLOW      = "\u001b[33m"
BLUE        = "\u001b[34m"
MAGENTA     = "\u001b[35m"
CYAN        = "\u001b[36m"
WHITE       = "\u001b[37m"

# GLOBAL VARIABLES

# This is a global variable to store myChip instance
global_myChip = 0

# Class Tile is the basic unit of showing info, representing the Tile data structure in cornerStitching
class Tile:
    def __init__(self):
        self.id = 0

        self.LL = 0
        self.LR = 0
        self.UL = 0
        self.UR = 0
        self.width = 0

        self.rt = 0
        self.tr = 0
        self.bl = 0
        self.lb = 0
        
# A Chip class is the data structure that holds all attributes, instantiate and link to global variable for
# full-scope access
class Chip:

    def __init__(self):
        self.chipName = ""
        self.chipWidth = -1
        self.chipHeight = -1
        self.tileCount = 0
        self.tileArr = []
        
        self.connCount = 0
        self.connArr = []
        self.dConn = []
        self.largestConn = 0
        self.connDict = dict()

        self.avName = True
        self.avBBox = True
        self.avVector = True
        self.avInfo = True
        self.avConn = True

        self.lineRead = 0

    def readChip(self, finCase):
        # Read the total number of Tiles 
        self.tileCount = int(finCase[self.lineRead])
        self.lineRead += 1
        
        for tileIdx in range(self.tileCount):
            tileHead = finCase[self.lineRead]
            self.lineRead += 1
            
            rtStr = finCase[self.lineRead]
            self.lineRead += 1
            trStr = finCase[self.lineRead]
            self.lineRead += 1
            blStr = finCase[self.lineRead]
            self.lineRead += 1
            lbStr = finCase[self.lineRead]
            self.lineRead += 1

            print("reading tile {}".format(tileIdx + 1))
            print("ID: ", tileHead)
            print("RT: ", rtStr)
            print("TR: ", trStr)
            print("BL: ", blStr)
            print("LB: ", lbStr)

            tile = Tile()
            tile.id = 0 
            

    def showChip(self):
        print("Printing attributes of the cornerStitching visualization:")
        print("There are total {} tiles to show".format(self.tileCount))

def parseTile(tileInfo):
    arr = tileInfo.split(',')
    blockType = arr[0].split

if __name__ == '__main__':

    # # A ColourGenerator class generates colors that are visually seperate, using precomputed LUT
    # colourG = ColourGenerator()

    # # Parse input arguments
    # parser = ArgumentParser()
    # parser.add_argument('-i', '--interactive', action='store_true', help='interactive mode', dest='interactiveFlag')
    # parser.add_argument('inFile', help='visualization input file')
    # parser.add_argument('-o', '--output', help='visualization output file', dest='outFile')
    # args = parser.parse_args()

    # print(CYAN, "IRISLAB Tile Rendering Program", COLORRST)
    # print("Input File: ", args.inFile)
    # if args.interactiveFlag == True:
    #     print("Interactive mode: ", GREEN, "ON", COLORRST)
    # else:
    #     print("Interactive mode: ", RED, "OFF", COLORRST)

    # if args.outFile == None:
    #     print("Rendering Results not saved")
    # else:
    #     print("Rendering Result saved to: ", args.outFile)
    
    # file_read_case = open(args.inFile, 'r')
    # fin_case = file_read_case.read().split("\n")

    # myChip = Chip()
    # myChip.readChip(fin_case)
    # myChip.showChip()
    str = "bl: T[BLOCK, R[(1180, 530), 320, 320, (1500, 850)]]"
    str = str.split(',') 
    blockType = str[0].split('[')[1]
    print(blockType)
    LLX = int(str[1].split('(')[1])
    LLY = int(str[2].split(')')[0])

    WIDTH = int(str[2])
    HE = int(str[2])

    print("LL is at ({}, {})".format(LLX, LLY))
    print(str)

    


