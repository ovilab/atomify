INCLUDEPATH += /projects/atomify/mobile/atomify/lammps-build/lammps-stable/src
LIBS += -L/projects/atomify/mobile/atomify/lammps-build/lammps-stable/src -llammps_ios_armv7 -llammps_ios_arm64

#LIBS += -L/projects/atomify/mobile/atomify/lammps-build/lammps-stable/src -llammps_kokkos_ios_arm64
#LIBS += -L/projects/atomify/mobile/atomify/lammps-build/lammps-stable/src/Obj_kokkos_ios_arm64/ -lkokkos -ldl  -L/usr/local/include

#QMAKE_APPLE_DEVICE_ARCHS = arm64
#EXPORT_VALID_ARCHS = arm64
