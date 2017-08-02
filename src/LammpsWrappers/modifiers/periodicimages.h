#ifndef PERIODICIMAGES_H
#define PERIODICIMAGES_H

#include "modifier.h"

class PeriodicImages : public Modifier
{
    Q_OBJECT
    Q_PROPERTY(int numberOfCopiesX READ numberOfCopiesX WRITE setNumberOfCopiesX NOTIFY numberOfCopiesXChanged)
    Q_PROPERTY(int numberOfCopiesY READ numberOfCopiesY WRITE setNumberOfCopiesY NOTIFY numberOfCopiesYChanged)
    Q_PROPERTY(int numberOfCopiesZ READ numberOfCopiesZ WRITE setNumberOfCopiesZ NOTIFY numberOfCopiesZChanged)
    Q_PROPERTY(bool showImagesX READ showImagesX WRITE setShowImagesX NOTIFY showImagesXChanged)
    Q_PROPERTY(bool showImagesY READ showImagesY WRITE setShowImagesY NOTIFY showImagesYChanged)
    Q_PROPERTY(bool showImagesZ READ showImagesZ WRITE setShowImagesZ NOTIFY showImagesZChanged)
    int m_numberOfCopiesX = 1;
    int m_numberOfCopiesY = 1;
    int m_numberOfCopiesZ = 1;
    bool m_showImagesX = true;
    bool m_showImagesY = true;
    bool m_showImagesZ = true;

public:
    PeriodicImages();
    virtual void apply(class AtomData &atomData) override;
    int numberOfCopiesX() const;
    int numberOfCopiesY() const;
    int numberOfCopiesZ() const;
    bool showImagesX() const;
    bool showImagesY() const;
    bool showImagesZ() const;

public slots:
    void setNumberOfCopiesX(int numberOfCopiesX);
    void setNumberOfCopiesY(int numberOfCopiesY);
    void setNumberOfCopiesZ(int numberOfCopiesZ);
    void setShowImagesX(bool showImagesX);
    void setShowImagesY(bool showImagesY);
    void setShowImagesZ(bool showImagesZ);

signals:
    void numberOfCopiesXChanged(int numberOfCopiesX);
    void numberOfCopiesYChanged(int numberOfCopiesY);
    void numberOfCopiesZChanged(int numberOfCopiesZ);
    void showImagesXChanged(bool showImagesX);
    void showImagesYChanged(bool showImagesY);
    void showImagesZChanged(bool showImagesZ);
};

#endif // PERIODICIMAGES_H
