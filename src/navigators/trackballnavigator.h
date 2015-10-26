#ifndef TRACKBALLNAVIGATOR_H
#define TRACKBALLNAVIGATOR_H

#include "../core/navigator.h"
#include <QVector3D>

class Visualizer;
class TrackballNavigator : public Navigator
{
    Q_OBJECT
    Q_PROPERTY(float zoomSensitivity READ zoomSensitivity WRITE setZoomSensitivity NOTIFY zoomSensitivityChanged)

public:
    explicit TrackballNavigator(Camera *camera = 0, QQuickItem *parent = 0);
    ~TrackballNavigator();
    float zoomSensitivity() const;

public slots:
    void setZoomSensitivity(float arg);

signals:
    void zoomSensitivityChanged(float arg);

protected:
    QVector2D m_touch1Position;
    QVector2D m_touch2Position;
    QVector3D m_initialCameraPosition;
    Qt::MouseButton m_button;
    float m_zoomSensitivity = 1.0;
    void pressed(QVector2D position);
    void moved(QVector2D delta);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void touchEvent(QTouchEvent *event);
    QVector2D scaledTouchPosition(QVector2D touchPosition);
};

#endif // TRACKBALLNAVIGATOR_H
