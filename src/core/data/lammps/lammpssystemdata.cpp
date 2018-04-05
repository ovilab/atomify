#include "lammpssystemdata.h"
#include <domain.h>
#include <library.h>

namespace atomify {

template <typename T>
T extract_global(void *lammps, const char *name) {
    void *data = lammps_extract_global(lammps, const_cast<char*>(name));
    return *static_cast<T*>(data);
}


void copy(LAMMPSSystemData *data, void *lammps)
{
    // Copy timestep and time
    double atime;
    uint64_t atimestep;
    data->dt = extract_global<double>(lammps, "dt");
    data->ntimestep = extract_global<uint64_t>(lammps, "ntimestep");
    atime = extract_global<double>(lammps, "atime");
    atimestep = extract_global<uint64_t>(lammps, "atimestep");
    data->time = atime + data->dt*(data->ntimestep - atimestep);

    // Copy triclinic status and atom count
    data->triclinic = extract_global<bool>(lammps, "triclinic");
    data->natoms = extract_global<uint64_t>(lammps, "natoms");
    data->ntypes = extract_global<int>(lammps, "ntypes");
    return;
    // Copy box size etc. TODO(anders.hafreager) create new library interface for this
    LAMMPS_NS::LAMMPS *lmp = static_cast<LAMMPS_NS::LAMMPS*>(lammps);
    auto c = lmp->domain->corners;

    data->origin[0] = c[0][0];
    data->origin[1] = c[0][1];
    data->origin[2] = c[0][2];

    data->size[0] = c[7][0] - data->origin[0]; // c[7] are end points
    data->size[1] = c[7][1] - data->origin[1];
    data->size[2] = c[7][2] - data->origin[2];

    data->center[0] = data->origin[0] + 0.5*data->size[0];
    data->center[1] = data->origin[1] + 0.5*data->size[1];
    data->center[2] = data->origin[2] + 0.5*data->size[2];

    data->cellMatrix[0][0] = c[1][0];
    data->cellMatrix[1][0] = c[1][1];
    data->cellMatrix[2][0] = c[1][2];

    data->cellMatrix[0][1] = c[2][0];
    data->cellMatrix[1][1] = c[2][1];
    data->cellMatrix[2][1] = c[2][2];

    data->cellMatrix[0][2] = c[4][0];
    data->cellMatrix[1][2] = c[4][1];
    data->cellMatrix[2][2] = c[4][2];

}

}  // namespace atomify
