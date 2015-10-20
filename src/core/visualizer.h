#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QQuickItem>
#include <QQuickFramebufferObject>
#include <QElapsedTimer>
#include <QDateTime>
#include "camera.h"

class Renderable; class Simulator; class SimulatorWorker; class Camera; class Navigator;

class VisualizerRenderer : public QQuickFramebufferObject::Renderer
{
public:
    Camera *camera() const;
    void setCamera(Camera *camera);

protected:
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void render();
    void synchronize(QQuickFramebufferObject *);
private:
    QList<Renderable*> m_renderables;
    QElapsedTimer m_elapsedTimer;
    Camera *m_camera = 0;
    unsigned int m_frameCount = 0;
    float m_fps = 60;
    qint64 m_fpsCounterTimeZero = QDateTime::currentMSecsSinceEpoch();
    QColor m_backgroundColor = QColor(0,0,0,0);
    bool m_visible = true;
};

class Visualizer : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(Simulator* simulator READ simulator WRITE setSimulator NOTIFY simulatorChanged)
    Q_PROPERTY(Camera* camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(Navigator* navigator READ navigator WRITE setNavigator NOTIFY navigatorChanged)
    Q_PROPERTY(float fps READ fps WRITE setFps NOTIFY fpsChanged)
    Q_PROPERTY(float time READ time NOTIFY timeChanged)
public:
    Visualizer();
    ~Visualizer();

    Simulator* simulator() const;
    Camera* camera();
    QColor backgroundColor() const;
    Navigator* navigator();
    float fps() const;
    float time() const;

public slots:
    void setSimulator(Simulator* arg);
    void setCamera(Camera* arg);
    void setBackgroundColor(QColor arg);
    void setNavigator(Navigator* arg);
    void setFps(float arg);
    void handleWindowChanged(QQuickWindow *win);

private slots:
    void synchronizeWorker(SimulatorWorker* worker);
    void resetAspectRatio();
    void timerTicked();

signals:
    void simulatorChanged(Simulator* arg);
    void cameraChanged(Camera* arg);
    void backgroundColorChanged(QColor arg);
    void navigatorChanged(Navigator* arg);
    void fpsChanged(float arg);
    void timeChanged(float arg);
    void touched();

private:
    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    Simulator* m_simulator = 0;
    Camera* m_camera = 0;
    Camera m_defaultCamera;
    Navigator* m_navigator = 0;
    QColor m_backgroundColor = QColor(0,0,0,0);
    float m_fps = 60;
    float m_time;

    friend class VisualizerRenderer;



protected:
    virtual VisualizerRenderer *createRenderer() const override;
};

#endif // VISUALIZER_H
