#include "lammpsatomdata.h"
#include <library.h>

namespace atomify {

void resize(LAMMPSAtomData *data, size_t size)
{
    data->x.resize(size);
    data->type.resize(size);
    data->mask.resize(size);
    data->id.resize(size);
}

void copy(LAMMPSAtomData *data, void *lammps)
{
    size_t natoms = static_cast<size_t>(lammps_get_natoms(lammps));
    resize(data, natoms);

    double **x = static_cast<double**>(lammps_extract_atom(lammps, const_cast<char*>("x")));
    int *type = static_cast<int *>(lammps_extract_atom(lammps, const_cast<char*>("type")));
    int *id = static_cast<int *>(lammps_extract_atom(lammps, const_cast<char*>("id")));
    int *mask = static_cast<int *>(lammps_extract_atom(lammps, const_cast<char*>("mask")));

    // LAMMPS data structure for atom positions is a double ** where each pointer
    // x[0], x[1] points to a one-dimensional allocated array laid out like
    // x1, y1, z1, x2, y2, z2, ..., xn, yn, zn. We can then safely copy 3n*sizeof(double) from x[0]

    memcpy(data->x.data(), x[0], natoms*3*sizeof(double));
    memcpy(data->type.data(), type, natoms*sizeof(int));
    memcpy(data->id.data(), id, natoms*sizeof(int));
    memcpy(data->mask.data(), mask, natoms*sizeof(int));
}

}  // namespace atomify
