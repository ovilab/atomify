#ifndef PERIODICIMAGES_H
#define PERIODICIMAGES_H

#include "modifier.h"



class PeriodicImages : public Modifier
{
    Q_OBJECT
    Q_PROPERTY(int numberOfCopiesX READ numberOfCopiesX WRITE setNumberOfCopiesX NOTIFY numberOfCopiesXChanged)
    Q_PROPERTY(int numberOfCopiesY READ numberOfCopiesY WRITE setNumberOfCopiesY NOTIFY numberOfCopiesYChanged)
    Q_PROPERTY(int numberOfCopiesZ READ numberOfCopiesZ WRITE setNumberOfCopiesZ NOTIFY numberOfCopiesZChanged)
    int m_numberOfCopiesX = 1;
    int m_numberOfCopiesY = 1;
    int m_numberOfCopiesZ = 1;

public:
    PeriodicImages(Atoms *atoms, System *system);

    // Modifier interface
public:
    virtual void apply(AtomData &atomData) override;
    int numberOfCopiesX() const;
    int numberOfCopiesY() const;
    int numberOfCopiesZ() const;

public slots:
    void setNumberOfCopiesX(int numberOfCopiesX);
    void setNumberOfCopiesY(int numberOfCopiesY);
    void setNumberOfCopiesZ(int numberOfCopiesZ);

signals:
    void numberOfCopiesXChanged(int numberOfCopiesX);
    void numberOfCopiesYChanged(int numberOfCopiesY);
    void numberOfCopiesZChanged(int numberOfCopiesZ);
};

#endif // PERIODICIMAGES_H
