# Atomify - a live LAMMPS visualizer #

[![Build Status](https://travis-ci.org/ovilab/atomify-lammps.svg?branch=dev)](https://travis-ci.org/ovilab/atomify-lammps)

The typical workflow when developing scripts for LAMMPS includes working with several programs. A text editor is needed to modify the scripts, the terminal to run LAMMPS, and programs like VMD or Ovito reading trajectories from a file dumped to the disk to visualize the system over time. If physical quantities are computed with LAMMPS, the data is often plotted with MATLAB or Python. This is a tedious process, especially for teaching purposes and for people who are new to LAMMPS. We here introduce Atomify, a high performance live visualizer for LAMMPS simulations with stunning graphics able to simulate and render more than 250000 atoms with good frame rate. Atomify supports OpenMP parallelization, GPU acceleration, live plotting of LAMMPS variables and computes and an easy to use code editor in one single program. The latter utilizes the powerful machinery already built into LAMMPS to allow easy access to advanced physical quantities. Atomify is open source software written in C++ built on top of Qt. 

## How to build ##
Step 1)
You will need Qt 5.7 with both Qt Charts and Qt Data Visualization. The easiest way to achieve this is to download Qt Creator from [https://www.qt.io/download/](https://www.qt.io/download/) (you can scroll down to the Open Source download section) and install it from there. Remember to include Qt Charts and Qt Data visualization when you install.

Step 2)
Clone the repository `git clone https://github.com/ovilab/atomify-lammps.git`.

Step 3)
Open the atomify-lammps directory and run `python configure.py` which will configure and compile LAMMPS. 

Step 4)
Open atomify-lammps.pro in Qt Creator and build/run.