#ifndef LAMMPSFILEHANDLER_H
#define LAMMPSFILEHANDLER_H
#include <QString>
#include <sstream>

namespace IO {
    QString readFile(QString filename);
    QString copyDataFileToReadablePath(QString filename);
    QString copyFileAndFixNewCommand(QString command, std::stringstream &commandStringStream);
}

#endif // LAMMPSFILEHANDLER_H
