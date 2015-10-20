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
#if defined(Q_OS_ANDROID)
    static ssize_t read(void *, char *, size_t);
    static ssize_t write(void *cookie, const char *buffer, size_t n);
    static int seek(void *cookie, off_t *__pos, int __w);
#elif defined(Q_OS_LINUX)
    static __ssize_t read(void *, char *, size_t);
    static __ssize_t write(void *cookie, const char *buffer, size_t n);
    static int seek(void *cookie, __off64_t *__pos, int __w);
#else
    static fpos_t seek(void *, fpos_t, int);
    static int write(void *cookie, const char *buffer, int size);
    static int read(void *, char *, int);
#endif
    static int clean(void *);
    FILE *m_filePointer = NULL;
    CPCompute* m_compute;
    friend class CPCompute;
    void parseVectorOutput(const QString &buffer);
    void parseScalarOutput(const QString &buffer);
};

#endif // LAMMPSOUTPUTPARSER_H
