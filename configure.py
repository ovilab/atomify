#!/usr/bin/python

import subprocess
import os
import sys
import shutil
from os.path import join, abspath

root_path = os.path.abspath(".")

def run_command(cmd):
    print cmd
    subprocess.call(cmd, shell=True)

if not os.path.exists("lammps-build"):
	os.makedirs("lammps-build")
os.chdir("lammps-build")
# run_command("wget http://lammps.sandia.gov/tars/lammps-stable.tar.gz")
if not os.path.exists("lammps-stable"):
	os.makedirs("lammps-stable")
# run_command("tar xvzf lammps-stable.tar.gz -C lammps-stable --strip-components 1")
os.chdir("lammps-stable")

os.chdir(root_path)

lammps_source_dir = join("lammps-build", "lammps-stable")
if not os.path.isdir(lammps_source_dir):
	print "Error, the path '"+lammps_source_dir+"' is not a directory."
	exit()

lammps_source_dir = abspath(lammps_source_dir)
lammps_source_dir_src = join(lammps_source_dir, "src")

lammps_pri = open("lammps.pri", "w")
lammps_pri.write("INCLUDEPATH += " + lammps_source_dir_src + "\n")
lammps_pri.write("INCLUDEPATH += " + lammps_source_dir_src + "/STUBS" + "\n")
lammps_pri.write("LIBS += -L" + lammps_source_dir_src + " -llammps_serial" + "\n")
lammps_pri.write("LIBS += -L" + lammps_source_dir_src + "/STUBS -lmpi_stubs" + "\n")

errorCppFile = join(lammps_source_dir_src, "error.cpp")
fix_ave_timeHFile = join(lammps_source_dir_src, "fix_ave_time.h")
if not os.path.isfile(errorCppFile):
	print "Error, could not find file to patch: "+errorCppFile
	print "Make sure you provided the correct LAMMPS folder."
	exit()

if not os.path.isfile(fix_ave_timeHFile):
	print "Error, could not find file to patch: "+fix_ave_timeHFile
	print "Make sure you provided the correct LAMMPS folder."
	exit()

print "Attempting to patch lammps source files error.cpp and fix_ave_time.h."
print "If these are already patched, just press enter (which by default answers 'no' on the questions) twice for each already patched file.\n"

errorCppPatchFile = join("lammps-patch", "error.cpp.patch")
run_command("patch %s < %s" % (errorCppFile, errorCppPatchFile)) 
fix_ave_timeHPatchFile = join("lammps-patch", "fix_ave_time.h.patch")
run_command("patch %s < %s" % (fix_ave_timeHFile, fix_ave_timeHPatchFile)) 
shutil.copy(join("lammps-patch", "lammpsexception.h"), lammps_source_dir_src)

print "\nLAMMPS was (probably) successfully patched."

print "\nCompiling LAMMPS"

os.chdir(lammps_source_dir_src)
run_command("make -j8 serial mode=lib")
os.chdir(root_path)
