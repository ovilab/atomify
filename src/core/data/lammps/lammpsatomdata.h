#ifndef CORE_DATA_LAMMPS_LAMMPSATOMDATA_H
#define CORE_DATA_LAMMPS_LAMMPSATOMDATA_H

#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <boost/pool/pool_alloc.hpp>

namespace atomify {

struct LAMMPSAtomData {
    std::vector<double> x;
    std::vector<int, boost::fast_pool_allocator<int>> type;
    std::vector<int, boost::fast_pool_allocator<int>> mask;
    std::vector<int, boost::fast_pool_allocator<int>> id;
    uint64_t size;
};

void resize(LAMMPSAtomData* data, size_t size);
void copy(LAMMPSAtomData* data, void* lammps);

} // namespace atomify

#endif // CORE_DATA_LAMMPS_LAMMPSATOMDATA_H
