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
    void parseLine(const char *string);

    QVector<CPCompute *> computes() const;
    void setComputes(const QVector<CPCompute *> &computes);

private:
    // These are functions allowing us to mimic a FILE* object
    static int clean(void *);
    static fpos_t seek(void *, fpos_t, int);
    static int write(void *cookie, const char *buffer, int size);
    static int read(void *, char *, int);

    FILE *m_filePointer = NULL;
    QVector<CPCompute*> m_computes;
};

#endif // LAMMPSOUTPUTPARSER_H
