#include "lammpsoutput.h"
#include <string>
#include <cassert>
#include <iostream>
#include <QString>

using namespace std;

LammpsOutput::LammpsOutput()
{
#ifdef Q_OS_LINUX

#else
    m_filePointer = funopen((const void*)this, LammpsOutput::read, LammpsOutput::write, LammpsOutput::seek, LammpsOutput::clean);
#endif
}


int LammpsOutput::read (void *cookie, char *buffer, int size) {

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

fpos_t LammpsOutput::seek (void *cookie, fpos_t position, int whence) {

    return 0;
}

int LammpsOutput::clean (void *cookie) {

    return 0;
}

FILE *LammpsOutput::file()
{
    return m_filePointer;
}

void LammpsOutput::parseLine(const char *string)
{
    QString qstr(string); // Convert to QString to do parsing easier
    QRegExp pattern("[ ]"); // Split by spaces
    QStringList list = qstr.split(pattern, QString::SkipEmptyParts); // List of each word in a line
//    if(list.size() != m_numberOfProperties) {
//        return; // This was an invalid line
//    }

    foreach(QString word, list) {
        bool isNumeric = false;
        if(word.toDouble(&isNumeric)) {

        } else {
            return; // This was an invalid line
        }
    }
}

bool LammpsOutput::childrenDirty() const
{
    return m_childrenDirty;
}

int LammpsOutput::frequency() const
{
    return m_frequency;
}


void LammpsOutput::childEvent(QChildEvent *)
{
    setChildrenDirty(true);
}


void LammpsOutput::setChildrenDirty(bool childrenDirty)
{
    if (m_childrenDirty == childrenDirty)
        return;

    m_childrenDirty = childrenDirty;
    emit childrenDirtyChanged(childrenDirty);
}

void LammpsOutput::setFrequency(int frequency)
{
    if (m_frequency == frequency)
        return;

    m_frequency = frequency;
    emit frequencyChanged(frequency);
}
