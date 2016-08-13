#ifndef DATA2D_H
#define DATA2D_H

#include <QHeightMapSurfaceDataProxy>
using namespace QtDataVisualization;

class Data2D : public QSurfaceDataProxy
{
    Q_OBJECT
    Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(float xMin READ xMin WRITE setXMin NOTIFY xMinChanged)
    Q_PROPERTY(float xMax READ xMax WRITE setXMax NOTIFY xMaxChanged)
    Q_PROPERTY(float yMin READ yMin WRITE setYMin NOTIFY yMinChanged)
    Q_PROPERTY(float yMax READ yMax WRITE setYMax NOTIFY yMaxChanged)
    Q_PROPERTY(float zMin READ zMin WRITE setZMin NOTIFY zMinChanged)
    Q_PROPERTY(float zMax READ zMax WRITE setZMax NOTIFY zMaxChanged)
public:
    explicit Data2D(QObject *parent = 0);
    enum Plane {XY, XZ, YZ};
    int plane = XY;
    QSize size() const;
    void setValue(float x, float y, float z);
    void update();
    float xMin() const;
    float xMax() const;
    float yMin() const;
    float yMax() const;
    float zMin() const;
    float zMax() const;

public slots:
    void setSize(QSize size);
    void setXMin(float xMin);
    void setXMax(float xMax);
    void setYMin(float yMin);
    void setYMax(float yMax);
    void setZMin(float zMin);
    void setZMax(float zMax);


signals:
    void sizeChanged(QSize size);
    void xMinChanged(float xMin);
    void xMaxChanged(float xMax);
    void yMinChanged(float yMin);
    void yMaxChanged(float yMax);
    void zMinChanged(float zMin);
    void zMaxChanged(float zMax);

private:
    QSize m_size;
    float m_xMin = 0;
    float m_xMax = 0;
    float m_yMin = 0;
    float m_yMax = 0;
    float m_zMin = 0;
    float m_zMax = 0;
    QSurfaceDataArray *m_dataArray = nullptr;
};

#endif // DATA2D_H
