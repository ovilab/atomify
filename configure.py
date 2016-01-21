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

lammps_build_type = "serial"
env = os.environ.copy()

# Set up SimVis
run_command("git submodule update --init --recursive")

# Set up LAMMPS

if _platform == "darwin":
    lammps_build_type = "serial"
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

os.chdir("lammps-build")
if not os.path.exists("lammps-stable.tar.gz"):
    run_command("curl \"http://lammps.sandia.gov/tars/lammps-stable.tar.gz\" -o \"lammps-stable.tar.gz\"")
if not os.path.exists("lammps-stable"):
    os.makedirs("lammps-stable")
    run_command("tar xzf lammps-stable.tar.gz -C lammps-stable --strip-components 1")
os.chdir(root_path)

if os.path.exists("lammps-patch/water"):
    for filename in glob.glob(os.path.join("lammps-patch/water", '*.*')):
        shutil.copy(filename, "lammps-build/lammps-stable/src/")

lammps_source_dir = join("lammps-build", "lammps-stable")
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
    lammps_pri.write("LIBS += -L" + lammps_source_dir_src + " -llammps_serial" + "\n")
    lammps_pri.write("LIBS += -L" + lammps_source_dir_src + "/STUBS -lmpi_stubs" + "\n")
    lammps_pri.close()

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

errorCppPatchFile = join(patch_path, "error.cpp.patch")
run_command("patch %s < %s" % (errorCppFile, errorCppPatchFile)) 
fix_ave_timeHPatchFile = join(patch_path, "fix_ave_time.h.patch")
run_command("patch %s < %s" % (fix_ave_timeHFile, fix_ave_timeHPatchFile)) 
shutil.copy(join(patch_path, "lammpsexception.h"), lammps_source_dir_src)

print "\nLAMMPS was (probably) successfully patched."

if lammps_build_type == "android":
    print "Compiling MPI stubs for Android"
    shutil.copy(join(patch_path, "Makefile.android"), join(lammps_source_dir_src, "MAKE/MACHINES"))
    shutil.copy(join(patch_path, "STUBS", "Makefile.android"), join(lammps_source_dir_src, "STUBS"))

    os.chdir(join(lammps_source_dir_src, "STUBS"))
    run_command("make -f Makefile.android")
    os.chdir(root_path)
        
print "\nCompiling LAMMPS"

os.chdir(lammps_source_dir_src)
run_command("make -j4 " + lammps_build_type + " mode=lib")
os.chdir(root_path)
