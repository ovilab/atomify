#include "trackballnavigator.h"
#include "../core/camera.h"
#include "../core/visualizer.h"
#include <cmath>
TrackballNavigator::TrackballNavigator(Camera *camera, QQuickItem *parent) :
    Navigator(camera, parent)
{

}

TrackballNavigator::~TrackballNavigator()
{

}

float TrackballNavigator::zoomSensitivity() const
{
    return m_zoomSensitivity;
}

void TrackballNavigator::setZoomSensitivity(float arg)
{
    if (m_zoomSensitivity == arg)
        return;

    m_zoomSensitivity = arg;
    emit zoomSensitivityChanged(arg);
}

QVector2D TrackballNavigator::scaledTouchPosition(QVector2D touchPosition) {
    float scaleFactor = std::min(width(), height());
    return QVector2D(touchPosition.x()/scaleFactor, touchPosition.y()/scaleFactor);
}

void TrackballNavigator::moved(QVector2D delta)
{
    if(m_button == Qt::RightButton) {
        QVector2D deltaScaled = -scaledTouchPosition(delta)*1e4;
        m_camera->translate(QVector3D(deltaScaled.x(), deltaScaled.y(), 0));
    } else {
        float deltaPan = -delta.x() * 200;
        float deltaTilt = -delta.y() * 200;

        double currentTilt = 180/M_PI*asin(double(m_camera->position().y()) / double(m_camera->position().length())); // sin(x) = a/b
        // First remove all tilt so panning is not biased. Note that y is flipped
        m_camera->tiltAboutViewCenter(currentTilt);
        m_camera->panAboutViewCenter(deltaPan);
        // Tilt back before we add the delta tilt from touch/mouse move
        m_camera->tiltAboutViewCenter(-currentTilt);
        m_camera->tiltAboutViewCenter(deltaTilt);
        emit m_camera->cameraMoved();
    }
}

void TrackballNavigator::mousePressEvent(QMouseEvent *event)
{
    Visualizer *myParent = qobject_cast<Visualizer*>(parent());
    if(myParent) {
        emit myParent->touched();
    }
    m_button = event->button();
    m_touch1Position = scaledTouchPosition(QVector2D(event->pos().x(), event->pos().y()));
}

void TrackballNavigator::mouseMoveEvent(QMouseEvent *event)
{
    QVector2D touch1Position = scaledTouchPosition(QVector2D(event->pos().x(), event->pos().y()));
    QVector2D delta = m_touch1Position - touch1Position;
    m_touch1Position = touch1Position;
    moved(-delta);
}

void TrackballNavigator::wheelEvent(QWheelEvent *event)
{
    float effectiveSensitivity = m_zoomSensitivity / 1080.0; // Typical deltaY is 120, scale by factor 1080
    float deltaY = -event->angleDelta().y() * effectiveSensitivity;
    float factor = exp(deltaY);
    m_camera->setPosition(m_camera->position()*factor);
    emit m_camera->cameraMoved();
}

void TrackballNavigator::touchEvent(QTouchEvent *event)
{
    int numberOfTouches = event->touchPoints().size();
    if(numberOfTouches >= 1) {
        const QTouchEvent::TouchPoint *touch1Point = &event->touchPoints().first();
        const QTouchEvent::TouchPoint *touch2Point = 0;
        if(numberOfTouches >= 2) {
            touch2Point = &event->touchPoints().at(1);
        }

        if(event->touchPointStates() & Qt::TouchPointMoved) {
            QVector2D delta1 = scaledTouchPosition(QVector2D(touch1Point->pos() - touch1Point->lastPos()));

            if(numberOfTouches >= 2) {
                QVector2D delta2 = scaledTouchPosition(QVector2D(touch2Point->pos() - touch2Point->lastPos()));

                // Only do pinching if we didn't release a touch
                if(!(event->touchPointStates() & Qt::TouchPointReleased)) {
                    QVector2D lastDelta = scaledTouchPosition(QVector2D(touch1Point->lastPos() - touch2Point->lastPos()));
                    QVector2D currentDelta = scaledTouchPosition(QVector2D(touch1Point->pos() - touch2Point->pos()));
                    float lastDistance = lastDelta.length();
                    float currentDistance = currentDelta.length();
                    float deltaDistance = currentDistance - lastDistance;
                    float factor = exp(-2.0*deltaDistance);
                    m_camera->setPosition(m_camera->position()*factor);

                    float lastTheta = atan2(lastDelta.y(), lastDelta.x());
                    float currentTheta = atan2(currentDelta.y(), currentDelta.x());
                    float deltaTheta = (lastTheta - currentTheta)*180.0/M_PI;
                    m_camera->rollAboutViewCenter(deltaTheta);
                    emit m_camera->cameraMoved();
                }

                // Require at least movement in both touches if we want to move
                if(delta1.length() > 0.002 && delta2.length() > 0.002) {
                    // Move average of the two deltas
                    moved(0.5*(delta1 + delta2));
                }
            } else {
                moved(delta1);
            }
        }
    }
}

