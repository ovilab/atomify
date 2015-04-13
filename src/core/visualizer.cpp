#include "visualizer.h"
#include "renderable.h"
#include "simulator.h"
#include "camera.h"
#include "navigator.h"
#include "../navigators/trackballnavigator.h"
#include <QDebug>
#include <QOpenGLFramebufferObjectFormat>

Visualizer::Visualizer() :
    m_defaultCamera(this)
{
    connect(this, &Visualizer::widthChanged, this, &Visualizer::resetAspectRatio);
    connect(this, &Visualizer::heightChanged, this, &Visualizer::resetAspectRatio);
    connect(this, &Visualizer::componentComplete, this, &Visualizer::resetAspectRatio);
    connect(&m_timer, &QTimer::timeout, this, &Visualizer::timerTicked);
    m_timer.start(16);
    m_elapsedTimer.start();

}

Visualizer::~Visualizer()
{

}

VisualizerRenderer *Visualizer::createRenderer() const
{
    return new VisualizerRenderer();
}

Simulator *Visualizer::simulator() const
{
    return m_simulator;
}

Camera *Visualizer::camera()
{
    if(!m_camera) {
        m_camera = &m_defaultCamera;
    }
    return m_camera;
}

QColor Visualizer::backgroundColor() const
{
    return m_backgroundColor;
}

Navigator *Visualizer::navigator()
{
    if(!m_navigator) {
        m_navigator = new Navigator(camera(), this);
    }
    return m_navigator;
}

float Visualizer::fps() const
{
    return m_fps;
}

float Visualizer::time() const
{
    return m_time;
}

void Visualizer::setSimulator(Simulator *arg)
{
    if (m_simulator == arg) {
        return;
    }
    if(m_simulator) {
        disconnect(m_simulator, &Simulator::requestVisualizerSync, this, &Visualizer::synchronizeWorker);
    }
    m_simulator = arg;
    connect(m_simulator, &Simulator::requestVisualizerSync, this, &Visualizer::synchronizeWorker);
    emit simulatorChanged(arg);
}

void Visualizer::setCamera(Camera *arg)
{
    if (m_camera == arg)
        return;

    m_camera = arg;
    emit cameraChanged(arg);
}

void Visualizer::setBackgroundColor(QColor arg)
{
    if (m_backgroundColor == arg)
        return;

    m_backgroundColor = arg;
    emit backgroundColorChanged(arg);
}

void Visualizer::setNavigator(Navigator *arg)
{
    if (m_navigator == arg)
        return;

    m_navigator = arg;
    emit navigatorChanged(arg);
}

void Visualizer::setFps(float arg)
{
    if (m_fps == arg)
        return;

    m_fps = arg;
    emit fpsChanged(arg);
}

void Visualizer::synchronizeWorker(SimulatorWorker *worker)
{
    QList<Renderable*> renderables = findChildren<Renderable*>();
    for(Renderable* renderable : renderables) {
        if(worker) {
            if(renderable->visible()) {
                worker->synchronizeRenderer(renderable);
            }
        }
    }
    update();
}

void Visualizer::resetAspectRatio()
{
    if(width() > 0 && height() > 0) {
        camera()->setAspectRatio(width() / height());
    }
}

void Visualizer::timerTicked()
{
    m_time = m_elapsedTimer.elapsed()*1e-3;
    emit timeChanged(m_time);
}

void VisualizerRenderer::render()
{
    QOpenGLFunctions funcs(QOpenGLContext::currentContext());

    funcs.glClearColor(m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF(), m_backgroundColor.alphaF());
    funcs.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    funcs.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    funcs.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    funcs.glFrontFace(GL_CW);
    funcs.glCullFace(GL_FRONT);
    funcs.glEnable(GL_CULL_FACE);
    funcs.glEnable(GL_DEPTH_TEST);

    for(Renderable* renderable : m_renderables) {
        if(renderable->visible()) {
            renderable->requestRender();
        }
    }

    funcs.glDepthMask(GL_TRUE);

    funcs.glDisable(GL_DEPTH_TEST);
    funcs.glDisable(GL_CULL_FACE);

    if(m_frameCount % 60 == 0 && m_frameCount > 0) {
        qint64 t1 = QDateTime::currentMSecsSinceEpoch();
        qint64 dt = t1 - m_fpsCounterTimeZero;
        m_fpsCounterTimeZero = QDateTime::currentMSecsSinceEpoch();
        m_fps = 60.0 / dt * 1000;
    }

    m_frameCount++;
}

void VisualizerRenderer::synchronize(QQuickFramebufferObject *fbo)
{
    Visualizer* visualizer = static_cast<Visualizer*>(fbo);
    m_renderables = visualizer->findChildren<Renderable*>();
    m_camera = visualizer->camera();
    m_backgroundColor = visualizer->backgroundColor();

    for(Renderable* renderable : m_renderables) {
        if(renderable->visible()) {
            if(!renderable->camera()) {
                renderable->setCamera(camera());
            }
            renderable->requestSynchronize();
        }
    }
    visualizer->setFps(m_fps);
}
Camera *VisualizerRenderer::camera() const
{
    return m_camera;
}

void VisualizerRenderer::setCamera(Camera *camera)
{
    m_camera = camera;
}


QOpenGLFramebufferObject *VisualizerRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
//    format.setSamples(4);
    QOpenGLFramebufferObject* fbo = new QOpenGLFramebufferObject(size, format);
    return fbo;
}
