#!/usr/local/bin/python


import os, re, sys, filecmp, getopt
from glob import glob
from random import randint
from sys import platform
import subprocess

global max_proc

def generate_rand_mat(dim):
	matA = open("test/matAR", "w")
	matB = open("test/matBR", "w")

	for row in range(0, dim):
		for col in range(0, dim):
			if (col < dim-1):
				matA.write(str(randint(0, 9)) + ",")
			else:
				matA.write(str(randint(0, 9)))

			if (col < dim-1):
				if (row == col):
					matB.write("1,")
				else:
					matB.write("0,")
			else:
				if (row == col):
					matB.write("1")
				else:
					matB.write("0")
		matA.write("\n")
		matB.write("\n")

	matA.close()
	matB.close()



if __name__ == '__main__':
	generate_rand_mat(int (sys.argv[1]))