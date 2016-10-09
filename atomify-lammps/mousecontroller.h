#ifndef MOUSECONTROLLER_H
#define MOUSECONTROLLER_H

#include <QObject>

class MouseController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool fpsMode READ fpsMode WRITE setFpsMode NOTIFY fpsModeChanged)
    bool m_fpsMode = false;

public:
    explicit MouseController(QObject *parent = 0);
    bool fpsMode() const;

signals:
    void fpsModeChanged(bool fpsMode);

public slots:
    void setFpsMode(bool fpsMode);
};

#endif // MOUSECONTROLLER_H
