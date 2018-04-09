#ifndef CORE_DATA_LAMMPS_LAMMPSDATA_H
#define CORE_DATA_LAMMPS_LAMMPSDATA_H

#include "./lammpsatomdata.h"
#include "./lammpssystemdata.h"
#include <iostream>

namespace atomify {

struct LAMMPSData {
    //    LAMMPSData()
    //    {
    //        std::cout << "Default constructor" << std::endl;
    //    }

    //    LAMMPSData(const LAMMPSData& other)
    //        : systemData(other.systemData)
    //        , atomData(other.atomData)
    //        , empty(other.empty)
    //    {
    //        std::cout << "Copy constructor" << std::endl;
    //    }

    //    LAMMPSData(LAMMPSData&& other)
    //        : systemData(std::move(other.systemData))
    //        , atomData(std::move(other.atomData))
    //        , empty(std::move(other.empty))
    //    {
    //        std::cout << "Move constructor" << std::endl;
    //    }

    //    LAMMPSData& operator=(const LAMMPSData& other)
    //    {
    //        std::cout << "Copy assignment" << std::endl;
    //        systemData = other.systemData;
    //        atomData = other.atomData;
    //        empty = other.empty;
    //        return *this;
    //    }

    //    LAMMPSData& operator=(LAMMPSData&& other)
    //    {
    //        std::cout << "Move assignment" << std::endl;
    //        systemData = std::move(other.systemData);
    //        atomData = std::move(other.atomData);
    //        empty = std::move(other.empty);
    //        return *this;
    //    }

    LAMMPSSystemData systemData;
    LAMMPSAtomData atomData;
    bool empty = true;
};

} // namespace atomify

#endif // CORE_DATA_LAMMPS_LAMMPSDATA_H
