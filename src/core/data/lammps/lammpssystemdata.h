#ifndef CORE_DATA_LAMMPS_LAMMPSSYSTEMDATA_H
#define CORE_DATA_LAMMPS_LAMMPSSYSTEMDATA_H
#include <array>
#include <string>

namespace atomify {

struct LAMMPSSystemData
{
    std::array<double, 3> size;
    std::array<double, 3> origin;
    std::array<double, 3> center;
    std::array<std::array<double, 3>, 3> cellMatrix;
    uint64_t ntimestep;
    double time;
    bool triclinic;
    uint64_t natoms;
    int ntypes;
    double dt;
};

void copy(LAMMPSSystemData *data, void *lammps);

}  // namespace atomify

#endif // CORE_DATA_LAMMPS_LAMMPSSYSTEMDATA_H
