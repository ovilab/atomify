#ifndef CORE_DATA_LAMMPS_LAMMPSDATA_H
#define CORE_DATA_LAMMPS_LAMMPSDATA_H

#include "./lammpssystemdata.h"
#include "./lammpsatomdata.h"
namespace atomify {

struct LAMMPSData {
    LAMMPSSystemData systemData;
    LAMMPSAtomData atomData;
    bool empty = true;
};

}  // namespace atomify

#endif // CORE_DATA_LAMMPS_LAMMPSDATA_H
