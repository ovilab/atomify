#include "lammpsoutput.h"
#include <string>
#include <cassert>
#include <iostream>
#include <QString>
#include <QDebug>
#include "CPcompute.h"

using namespace std;

LammpsOutput::LammpsOutput()
{
#ifdef Q_OS_LINUX

#else
    m_filePointer = funopen((const void*)this, LammpsOutput::read, LammpsOutput::write, LammpsOutput::seek, LammpsOutput::clean);
#endif
}


int LammpsOutput::read (void *, char *, int ) {

}

int LammpsOutput::write (void *cookie, const char *buffer, int size) {
    LammpsOutput *parser = (LammpsOutput*)cookie;
    parser->parse(QString(buffer));
}

fpos_t LammpsOutput::seek (void *, fpos_t , int ) {

    return 0;
}

int LammpsOutput::clean (void *) {

    return 0;
}

FILE *LammpsOutput::stream()
{
    return m_filePointer;
}

void LammpsOutput::parse(QString buffer)
{
    QStringList lines = buffer.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

//    qDebug() << "Got data: " << buffer;
//    qDebug() << "List now has " << lines.size() << ": " << lines;

    foreach(const QString &line, lines) {
        QRegExp pattern("[ ]"); // Split by spaces
        QStringList list = line.split(pattern, QString::SkipEmptyParts); // List of each word in a line
        QVector<double> numericalOutput;
        bool invalidLine = false;

        bool firstWord = true;
        foreach(const QString &word, list) {
            bool isNumerical;
            double value = word.toDouble(&isNumerical);
            if(!isNumerical) {
                invalidLine = true;
                break;
            }

            if(firstWord) {
                // The first word is always the timestep number, we'll skip that
                firstWord = false;
                continue;
            }

            numericalOutput.push_back(value);
        }

        if(!invalidLine) {
            // qDebug() << "Our list is now: " << numericalOutput;
            m_compute->setValues(numericalOutput);
        }
    }

}
