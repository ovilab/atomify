# Atomify - a live LAMMPS visualizer #

[![Build Status](https://travis-ci.org/ovilab/atomify.svg?branch=dev)](https://travis-ci.org/ovilab/atomify)
[![Join the chat at https://gitter.im/ovilab](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/ovilab/atomify?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

The typical workflow when developing scripts for LAMMPS includes working with several programs. A text editor is needed to modify the scripts, the terminal to run LAMMPS, and programs like VMD or Ovito reading trajectories from a file dumped to the disk to visualize the system over time. If physical quantities are computed with LAMMPS, the data is often plotted with MATLAB or Python. This is a tedious process, especially for teaching purposes and for people who are new to LAMMPS. We here introduce Atomify, a high performance live visualizer for LAMMPS simulations with stunning graphics able to simulate and render more than 250000 atoms with good frame rate. Atomify supports OpenMP parallelization, GPU acceleration, live plotting of LAMMPS variables and computes and an easy to use code editor in one single program. The latter utilizes the powerful machinery already built into LAMMPS to allow easy access to advanced physical quantities. Atomify is open source software written in C++ built on top of Qt. 

![Atomify lets you run LAMMPS and visualize the state live](https://github.com/ovilab/atomify/raw/dev/screenshots/screenshot.png "Atomify lets you run LAMMPS and visualize the state live")


## How to install ##
### MacOS ###
Alternative 1) Download [the pkg](https://github.com/ovilab/atomify/releases/download/v2.0.9/Atomify-2.0.9-macos.pkg) (preferred).

Alternative 2) 
Install with Homebrew: `brew install https://raw.githubusercontent.com/ovilab/atomify/dev/macos/atomify.rb`.

Alternative 3) Download from [Mac App Store](https://itunes.apple.com/us/app/atomify/id1192327034?mt=12).

### Linux ###
`sudo snap install atomify`

### Windows ###
Coming soon.


## How to build ##
If you have Homebrew (macOS), you can install with `brew install https://raw.githubusercontent.com/ovilab/atomify/dev/macos/atomify.rb`.

Step 1)
You will need Qt 5.9. The easiest way to achieve this is to download Qt Creator from [https://www.qt.io/download-open-source/](https://www.qt.io/download-open-source/) and install it from there. When you run the installer, you can just press skip when it asks you for the account. If you are using Mac, you can also uncheck the ~10GB iOS package unless you want that.

**If you are on Ubuntu, you will also need OpenGL libraries. You can achieve this by running `sudo apt install libgl1-mesa-dev`.**

Step 2)
Clone the repository `git clone --recursive https://github.com/ovilab/atomify.git`

Step 3)
Open the atomify directory and run `python configure.py` which will configure and compile LAMMPS. 

Step 4)
Open atomify.pro in Qt Creator and build/run (remember to choose Release mode for better performance).

## How to add/remove LAMMPS packages ##
If you compiled Atomify yourself, you can easily modify the LAMMPS installation (packages and your own code).
LAMMPS is located in libs/lammps. You need to recompile LAMMPS with (being in the libs/lammps/src folder)
`make atomify mode=lib`. Then you need to recompile Atomify, but Qt won't detect your changes unless you modify i.e. main.cpp (just add a line and save to modify date). 