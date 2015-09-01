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
QVector<CPCompute *> LammpsOutput::computes() const
{
    return m_computes;
}

void LammpsOutput::setComputes(const QVector<CPCompute *> &computes)
{
    m_computes = computes;
}


int LammpsOutput::write (void *cookie, const char *buffer, int size) {
    LammpsOutput *parser = (LammpsOutput*)cookie;

    const char *currentLine = buffer;
    while(currentLine) {
        char *nextLine = strchr(currentLine, '\n');
        if(nextLine) *nextLine = '\0';
        parser->parseLine(currentLine);
        if(nextLine) *nextLine = '\n';
        currentLine = nextLine ? (nextLine+1) : NULL;
    }
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

void LammpsOutput::parseLine(const char *string)
{
    QString qstr(string); // Convert to QString to do parsing easier
    QRegExp pattern("[ ]"); // Split by spaces
    QStringList list = qstr.split(pattern, QString::SkipEmptyParts); // List of each word in a line
    int numberOfExpectedOutputWords = m_computes.size() + 2; // Step Time compute1 compute2 ...
    if(list.size() != numberOfExpectedOutputWords || list.size() <= 2) {
        return; // This line did not contain Step Time compute1 compute2 ...
    }

    QVector<double> numericalOutput;

    foreach(QString word, list) {
        bool isNumeric = false;
        if(word.toDouble(&isNumeric)) {
            numericalOutput.push_back(word.toDouble());
            // qDebug() << "We have a number: "  << word;
        } else {
            return; // This was an invalid line
        }
    }

    if(numericalOutput.size() == numberOfExpectedOutputWords) {
        // This is probably the output we want (unless we have false positives, TODO: FIX THIS)
        // unsigned int timestep = numericalOutput[0];
        double time = numericalOutput[1];
        for(int i=2; i<numericalOutput.size(); i++) {
            int computeArrayIndex = i-2; // The two first values in the output don't have a compute assigned to them (Step and Time)
            CPCompute *compute = m_computes[computeArrayIndex];
            double value = numericalOutput[i];
            compute->setValue(qMakePair<double,double>(time, value));
        }
    }
}
