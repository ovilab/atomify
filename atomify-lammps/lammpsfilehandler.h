#ifndef LAMMPSFILEHANDLER_H
#define LAMMPSFILEHANDLER_H
#include <QString>
#include <sstream>

QString readFile(QString filename);
QString copyDataFileToReadablePath(QString filename);
QString copyFileAndFixNewCommand(QString command, std::stringstream &commandStringStream);

#endif // LAMMPSFILEHANDLER_H
