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

int LammpsOutput::numberOfExpectedOutputs()
{
    int num = 2; // The first two are Step and Time
    for(CPCompute *compute : m_computes) {
        num += compute->numProperties();
    }

    return num;
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

    fprintf(stdout, buffer);
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
    int numberOfExpectedOutputWords = numberOfExpectedOutputs();

    if(list.size() != numberOfExpectedOutputWords || list.size() <= 2) {
        return; // This line did not match Step Time compute1 compute2 ...
    }

    QVector<double> numericalOutput;

    foreach(QString word, list) {
        bool isNumeric = false;
        if(word.toDouble(&isNumeric)) {
            numericalOutput.push_back(word.toDouble());
        } else {
            return; // This was a line not matching all numbers
        }
    }

    if(numericalOutput.size() == numberOfExpectedOutputWords) {
        // This is probably the output we want (unless we have false positives, TODO: FIX THIS)
        double time = numericalOutput[1];
        // We might have an output matching: Step Time msd[1] msd[2] msd[3] msd[4] temperature pressure
        // with values                       10   0.1  0.001  0.021  0.003  0.021  1.44        0.89
        // We now loop through all values (skipping the first two). We see that the first 4 values are msd values (which here will be the first compute in the list).
        // The compute knows how many output it expects, so when its list is filled up, the next compute will be the next in the list.
        CPCompute *currentCompute = m_computes.first();
        int currentComputeArrayIndex = 0;
        QVector<double> valuesForCurrentCompute;
        for(int i=2; i<numericalOutput.size(); i++) {
            double value = numericalOutput[i];
            valuesForCurrentCompute.push_back(value);

            if(valuesForCurrentCompute.size() == currentCompute->numProperties()) {
                currentCompute->setValues(time, valuesForCurrentCompute);
                valuesForCurrentCompute.clear(); // Reset this list

                // If there are any computes left after this, update the pointer
                if(m_computes.size() > currentComputeArrayIndex+1) {
                    currentCompute = m_computes.at(++currentComputeArrayIndex); // Pick the next compute
                }
            }
        }
    }
}
