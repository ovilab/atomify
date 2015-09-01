#include "lammpsfilehandler.h"
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDebug>

QString readFile(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open file: " << file.fileName();
        return "";
    }

    return file.readAll();
}


QString copyDataFileToReadablePath(QString filename)
{
    QString qrcFilename = ":/scripts/"+filename;

    bool fileFound = false;
    QFileInfo fileInfo(qrcFilename);
    fileFound = fileInfo.exists();

    if(!fileFound) {
        // The file could be a zipped file, let's check that
        qrcFilename.append(".gz");
        filename.append(".gz");
        fileInfo = QFileInfo(qrcFilename);
        fileFound = fileInfo.exists();
    }

    QString tempLocation = QStandardPaths::locate(QStandardPaths::TempLocation, QString(), QStandardPaths::LocateDirectory);
    QString newFilename=tempLocation+filename;
    if(fileFound) {
        QFile::copy(qrcFilename, newFilename);
    }

    return newFilename;
}


QString copyFileAndFixNewCommand(QString command, std::stringstream &commandStringStream)
{

}
