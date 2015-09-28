//#include "fileio.h"

//FileIO::FileIO(QObject *parent) : QObject(parent)
//{
//    QFile file(filename);
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        qDebug() << "Could not open file: " << file.fileName();
//        return "";
//    }

//    return file.readAll();
//}

//void FileIO::saveToFile(QString filename, QString content)
//{
//    QFile file(filename);
//    QByteArray array = content.toUtf8();
//    file.write(array);
//}

//QString FileIO::readFile(QString filename)
//{

//}

