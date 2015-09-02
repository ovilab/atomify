#ifndef LAMMPSOUTPUTPARSER_H
#define LAMMPSOUTPUTPARSER_H
#include <stdio.h>
#include <QObject>
#include <QVector>
#include <QVariantList>

class CPCompute;
class LammpsOutput : public QObject
{
    Q_OBJECT
public:
    LammpsOutput();
    FILE *stream();
    void parse(QString buffer);
private:
    // These are functions allowing us to mimic a FILE* object
    static int clean(void *);
    static fpos_t seek(void *, fpos_t, int);
    static int write(void *cookie, const char *buffer, int size);
    static int read(void *, char *, int);
    FILE *m_filePointer = NULL;
    CPCompute* m_compute;
    friend class CPCompute;
};

#endif // LAMMPSOUTPUTPARSER_H
