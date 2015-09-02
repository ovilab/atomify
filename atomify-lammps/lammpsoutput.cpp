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

void LammpsOutput::parseVectorOutput(const QString &buffer) {
    QStringList lines = buffer.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

    unsigned int timestep = 0;
    int numberOfValues = 0;
    QVector<double> numericalOutput;

    bool isFirstLine = true;
    foreach(const QString &line, lines) {
        bool invalidLine = false;
        QStringList words = line.split(QRegExp("[ ]"), QString::SkipEmptyParts); // Split by spaces.

        bool isFirstWord = true;
        foreach(const QString &word, words) {
            bool isNumerical;
            double value = word.toDouble(&isNumerical);
            if(!isNumerical) {
                // Could not parse to double. Then this line is nothing we want.
                invalidLine = true;
                break;
            }

            if(isFirstLine) {
                // This line contains 'Timestep NumValues'
                if(isFirstWord) {
                    timestep = value;
                    isFirstWord = false;
                } else {
                    numberOfValues = value;
                }
            } else {
                // This line contains ValueId Value
                if(isFirstWord) {
                    // First word is the value id (1,2,...)
                    isFirstWord = false;
                } else {
                    // This is our value
                    numericalOutput.push_back(value);
                }
            }
        }

        if(isFirstLine) isFirstLine = false;
    }

    m_compute->setValues(numericalOutput);
}

void LammpsOutput::parseScalarOutput(const QString &buffer) {
    QStringList lines = buffer.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

    unsigned int timestep = 0;
    foreach(const QString &line, lines) {
        bool invalidLine = false;

        QStringList words = line.split(QRegExp("[ ]"), QString::SkipEmptyParts); // Split by spaces.
        QVector<double> numericalOutput;

        bool isFirstWord = true;
        foreach(const QString &word, words) {
            bool isNumerical;
            double value = word.toDouble(&isNumerical);
            if(!isNumerical) {
                // Could not parse to double. Then this line is nothing we want.
                invalidLine = true;
                break;
            }

            if(isFirstWord) {
                // First word in fix outputs is always timestep
                timestep = value;
                isFirstWord = false;
            } else numericalOutput.push_back(value);
        }

        if(!invalidLine) {
            m_compute->setValues(numericalOutput);
        }
    }
}

void LammpsOutput::parse(QString buffer)
{
    if(m_compute->isVector()) parseVectorOutput(buffer);
    else parseScalarOutput(buffer);

}
