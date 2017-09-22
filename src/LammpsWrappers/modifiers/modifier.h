#ifndef MODIFIER_H
#define MODIFIER_H
#include <QObject>

class Modifier : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
protected:
    class System *m_system = nullptr;

public:
    Modifier();
    virtual void apply(class AtomData &atomData) = 0;
    bool enabled() const;
    void setSystem(class System *system);
    Q_INVOKABLE virtual void parseCommand(QString cmd) {}

public slots:
    void setEnabled(bool enabled);

signals:
    void enabledChanged(bool enabled);
protected:

    bool m_enabled = false;
};

#endif // MODIFIER_H
