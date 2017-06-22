#!/usr/bin/python

import subprocess
import os
import sys
import shutil
from os.path import join, abspath
from sys import platform as _platform
import glob
def run_command(cmd):
    print cmd
    subprocess.call(cmd, shell=True, env=env)

lammps_build_type = "atomify"
env = os.environ.copy()
currentPath = os.getcwd()
# Set up examples
run_command("git submodule update --init --recursive")
os.chdir(os.path.join("src","examples"))
run_command("python generateExamples.py")
os.chdir(currentPath)
# Set up LAMMPS
ompSupport = True

if _platform == "darwin":
    lammps_build_type = "atomify"
    ompSupport = False
elif _platform == "win32":
    print "Windows is not supported yet"
    exit()
elif len(sys.argv) > 1 and sys.argv[1] == "android":
    lammps_build_type = "android"
    if len(sys.argv) < 4:
        print "You need to specify the Android NDK path and Android ABI version (for example \"4.9\")"
        print "python configure.py android <Android NDK path> <Android ABI version>"
        print "Example:"
        print "python configure.py android /home/username/apps/android-ndk-r10d 4.9"
        exit()
    env["ANDROID_NDK_PATH"] = sys.argv[2]
    env["ANDROID_ABI"] = sys.argv[3]
    
root_path = os.path.abspath(".")
patch_path = join(root_path, "lammps-patch")

if not os.path.exists("lammps-build"):
    os.makedirs("lammps-build")

lammps_source_dir = join("libs", "lammps")
#if not os.path.exists("lammps"):
#    run_command("git clone https://github.com/ovilab/lammps.git")
#if not os.path.exists("lammps"):
#    print("Error, could not clone LAMMPS from github. Please make sure you have git installed.")
#    exit(1)
os.chdir(root_path)

if os.path.exists("lammps-patch/water"):
    for filename in glob.glob(os.path.join("lammps-patch/water", '*.*')):
        shutil.copy(filename, "lammps-build/lammps/src/")

if not os.path.isdir(lammps_source_dir):
    print "Error, the path '"+lammps_source_dir+"' is not a directory."
    exit()

lammps_source_dir = abspath(lammps_source_dir)
lammps_source_dir_src = join(lammps_source_dir, "src")

if lammps_build_type == "android":
    lammps_android_pri = open("lammps-android.pri", "w")
    lammps_android_pri.write("INCLUDEPATH += " + lammps_source_dir_src + "\n")
    lammps_android_pri.write("INCLUDEPATH += " + lammps_source_dir_src + "/STUBS" + "\n")
    lammps_android_pri.write("LIBS += -L" + lammps_source_dir_src + " -llammps_android" + "\n")
    lammps_android_pri.write("LIBS += -L" + lammps_source_dir_src + "/STUBS -lmpi_stubs_android" + "\n")
    lammps_android_pri.close()
else:
    lammps_pri = open("lammps.pri", "w")
    lammps_pri.write("INCLUDEPATH += " + lammps_source_dir_src + "\n")
    lammps_pri.write("INCLUDEPATH += " + lammps_source_dir_src + "/STUBS" + "\n")
    lammps_pri.write("LIBS += -L" + lammps_source_dir_src + " -llammps_atomify" + "\n")
    lammps_pri.write("LIBS += -L" + lammps_source_dir_src + "/STUBS -lmpi_stubs" + "\n")
    lammps_pri.close()
    
#errorCppFile = join(lammps_source_dir_src, "error.cpp")
fix_ave_timeHFile = join(lammps_source_dir_src, "fix_ave_time.h")

#if not os.path.isfile(errorCppFile):
#    print "Error, could not find file to patch: "+errorCppFile
#    print "Make sure you provided the correct LAMMPS folder."
#    exit()

if not os.path.isfile(fix_ave_timeHFile):
    print "Error, could not find file to patch: "+fix_ave_timeHFile
    print "Make sure you provided the correct LAMMPS folder."
    exit()

print "Attempting to patch lammps source files"
print "If these are already patched, just press enter (which by default answers 'no' on the questions) twice for each already patched file.\n"

for filename in glob.glob(join(patch_path, "*.patch")):
    basename = os.path.basename(filename)
    print("Patching", basename)
    patch_file = join(patch_path, basename)
    original_file = join(lammps_source_dir_src, basename.replace(".patch", ""))
    run_command("patch %s < %s" % (original_file, patch_file)) 

if ompSupport:
    shutil.copy(join(patch_path, "Makefile.atomify-omp"), join(lammps_source_dir_src, "MAKE", "Makefile.atomify"))
else:
    shutil.copy(join(patch_path, "Makefile.atomify"), join(lammps_source_dir_src, "MAKE", "Makefile.atomify"))

print "\nLAMMPS was (probably) successfully patched."

if lammps_build_type == "android":
    print "Compiling MPI stubs for Android"
    shutil.copy(join(patch_path, "Makefile.android"), join(lammps_source_dir_src, "MAKE/MACHINES"))
    shutil.copy(join(patch_path, "STUBS", "Makefile.android"), join(lammps_source_dir_src, "STUBS"))

    os.chdir(join(lammps_source_dir_src, "STUBS"))
    run_command("make clean")
    run_command("make -f Makefile.android")
    os.chdir(root_path)
else:
    os.chdir(join(lammps_source_dir_src, "STUBS"))
    run_command("make")

        
print "\nCompiling LAMMPS"

os.chdir(lammps_source_dir_src)
run_command("make yes-rigid yes-manybody yes-mc yes-molecule yes-granular yes-replica yes-kspace yes-shock yes-misc yes-USER-MISC yes-user-reaxc")
if ompSupport:
    run_command("make yes-user-omp")

# Copy moltemplate extra files
for filename in glob.iglob(os.path.join(patch_path, "moltemplate_additional_lammps_code", "*.cpp")):
    print("Copying ", filename)
    shutil.copy2(filename, lammps_source_dir_src)
for filename in glob.iglob(os.path.join(patch_path, "moltemplate_additional_lammps_code", "*.h")):
    print("Copying ", filename)
    shutil.copy2(filename, lammps_source_dir_src)
    
run_command("make -j4 " + lammps_build_type + " mode=lib")
os.chdir(root_path)
