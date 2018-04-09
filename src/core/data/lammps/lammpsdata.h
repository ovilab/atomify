#ifndef CORE_DATA_LAMMPS_LAMMPSDATA_H
#define CORE_DATA_LAMMPS_LAMMPSDATA_H

#include "./lammpsatomdata.h"
#include "./lammpssystemdata.h"

namespace atomify {

struct LAMMPSData {
    LAMMPSSystemData systemData;
    LAMMPSAtomData atomData;
};

} // namespace atomify

#endif // CORE_DATA_LAMMPS_LAMMPSDATA_H
