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
    ~LammpsOutput();
    FILE *stream();
    void parse(QString buffer);
private:
    // These are functions allowing us to mimic a FILE* object
    static int clean(void *);
    static fpos_t seek(void *, fpos_t, int);
    static int write(void *cookie, const char *buffer, int size);
    static int read(void *, char *, int);
    static __ssize_t readLinux(void *, char *, size_t);
    static __ssize_t writeLinux(void *cookie, const char *buffer, size_t n);
    static int seekLinux(void *cookie, __off64_t *__pos, int __w);
    FILE *m_filePointer = NULL;
    CPCompute* m_compute;
    friend class CPCompute;
    void parseVectorOutput(const QString &buffer);
    void parseScalarOutput(const QString &buffer);
};

#endif // LAMMPSOUTPUTPARSER_H
