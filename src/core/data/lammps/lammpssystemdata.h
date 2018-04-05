#ifndef LAMMPSSYSTEMDATA_H
#define LAMMPSSYSTEMDATA_H
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

}

#endif // LAMMPSSYSTEMDATA_H
