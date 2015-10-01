#!/usr/bin/python

import subprocess
import os
import sys

def run_command(cmd):
    print cmd
    subprocess.call(cmd, shell=True)

numArgs = len(sys.argv)
if(numArgs == 1):
	print "Please provide link to lammps source folder. For instance:"
	print "python patch_lammps.python /projects/lammps-10-august/src/"
	exit()

lammpsSourceDir = sys.argv[1]
if not os.path.isdir(lammpsSourceDir):
	print "Error, the path '"+lammpsSourceDir+"' is not a directory."
	exit()

errorCppFile = lammpsSourceDir+"/error.cpp"
fix_ave_timeHFile = lammpsSourceDir+"/fix_ave_time.h"
if not os.path.isfile(errorCppFile):
	print "Error, could not find file to patch: "+errorCppFile
	print "Make sure you provided the correct LAMMPS source folder."
	exit()

if not os.path.isfile(fix_ave_timeHFile):
	print "Error, could not find file to patch: "+fix_ave_timeHFile
	print "Make sure you provided the correct LAMMPS source folder."
	exit()

print "Attempting to patch lammps source files error.cpp and fix_ave_time.h."
print "If these are already patched, just press enter (which by default answers 'no' on the questions) twice for each already patched file.\n"

errorCppPatchFile = "./error.cpp.patch"
run_command("patch %s < %s" % (errorCppFile, errorCppPatchFile)) 
fix_ave_timeHPatchFile = "./fix_ave_time.h.patch"
run_command("patch %s < %s" % (fix_ave_timeHFile, fix_ave_timeHPatchFile)) 

print "\nLAMMPS was (probably) successfully patched. Note: you also need to compile LAMMPS as a library with c++11. Se README.md for instructions."