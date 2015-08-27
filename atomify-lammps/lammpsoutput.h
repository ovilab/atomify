#ifndef LAMMPSOUTPUTPARSER_H
#define LAMMPSOUTPUTPARSER_H
#include <stdio.h>
#include <QObject>
#include <QVariantList>
class LammpsOutput : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool childrenDirty READ childrenDirty WRITE setChildrenDirty NOTIFY childrenDirtyChanged)
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
public:
    LammpsOutput();
    FILE *file();
    void parseLine(const char *string);
    bool childrenDirty() const;
    int frequency() const;

public slots:
    void setChildrenDirty(bool childrenDirty);
    void setFrequency(int frequency);

signals:
    void childrenDirtyChanged(bool childrenDirty);
    void frequencyChanged(int frequency);

protected:
    FILE *m_filePointer = NULL;
    bool m_childrenDirty = false;
    int m_frequency = 1;
    // QObject interface
    virtual void childEvent(QChildEvent *);

private:
    static int clean(void *cookie);
    static fpos_t seek(void *cookie, fpos_t position, int whence);
    static int write(void *cookie, const char *buffer, int size);
    static int read(void *cookie, char *buffer, int size);
};

#endif // LAMMPSOUTPUTPARSER_H
