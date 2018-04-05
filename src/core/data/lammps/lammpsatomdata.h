#ifndef CORE_DATA_LAMMPS_LAMMPSATOMDATA_H
#define CORE_DATA_LAMMPS_LAMMPSATOMDATA_H

#include <cstdint>
#include <vector>

namespace atomify {

struct LAMMPSAtomData
{
    std::vector<double> x;
    std::vector<int> type;
    std::vector<int> mask;
    std::vector<int> id;
    uint64_t size;
};

void resize(LAMMPSAtomData *data, size_t size);
void copy(LAMMPSAtomData *data, void *lammps);

}  // namespace atomify

#endif // CORE_DATA_LAMMPS_LAMMPSATOMDATA_H
