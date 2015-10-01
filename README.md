#Atomify - a live LAMMPS visualizer
*Simplifty your workflow significantly.* Atomify is a script editor for LAMMPS supporting live visualization of the simulation. Edit your script, just press Run, and you will immediately see the atoms on your screen while LAMMPS is running. There is no need to dump the trajectories to disk and open them in VMD or Ovito anymore. It uses the high performance visualization library **[SimVis](https://github.com/computationalphysics/SimVis)** (based on Qt) allowing live visualization of millions of atoms in a live simulation. 

##Why
The current workflow for developing LAMMPS scripts is as follows:
- Edit the script in your favorite text editor
- Open terminal, run the script and dump trajectories to disk
- Open VMD, Ovito or some other visualization software to visualize the dumped trajectories
With Atomify, we solved this problem by combining the script editing and visualizing in the same tool. Atomify runs LAMMPS internally, so there is almost no overhead in visualizing the simulation. 

##Highlights
- Full OpenMP support (MPI support will come later)
- Also runs on your mobile phone / tablet (both Android and iOS)

##Installation
* Atomify is based on the Qt library, so you need to download **[Qt Creator](http://www.qt.io/download-open-source/)** (the open source version is fine). Qt version 5.5.0 or later is required.
* Download the **[LAMMPS source code](http://lammps.sandia.gov/download.html)**
It is now assumed that <atomify_source_folder> is the folder containing this README.md file. It is also assumed that <lammps_source_folder> is the folder containing the LAMMPS source code. This is for instance '~/Downloads/lammps-10Aug15/src'.
Once you have downloaded the LAMMPS source code and extracted the contents, you will need to patch the LAMMPS source code with the patch_lammps.py python script:
- `cd <atomify_source_folder>`
- `python patch_lammps.py <lammps_source_folder>`

Then, you will need to compile the LAMMPS source code with c++11 support. To do this, open the Makefile you will use (in <lammps_source_folder/MAKE/OPTIONS/Makefile.yourfavorite) and add '-std=c++11' to the CCFLAGS line. For instance, in the Makefile.ompi_g++, you should have the line `CCFLAGS =	-g -O3 -std=c++11`. 
Now compile LAMMPS as a library. For example, to make the ompi_g++ version of LAMMPS:
- `cd <lammps_source_folder>`
- `make ompi_g++ mode=lib`

The last step is to open Atomify in Qt Creator (open <atomify_source_folder>/atomify-lammps.pro). In Qt Creator, open the atomify-lammps.pro file and edit the two lines
`INCLUDEPATH += /projects/lammps-10Aug15/src`
`LIBS += -L/projects/lammps-10Aug15/src -llammps_omp`
to match your <lammps_source_folder> and which version you compiled. For instance, if you compiled the ompi_g++ version, the linking in the last line should be `-llammps_ompi_g++`.

Now, choose Release mode and enjoy Atomify.