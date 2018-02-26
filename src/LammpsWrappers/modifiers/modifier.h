#ifndef MODIFIER_H
#define MODIFIER_H
#include <QObject>
#include <QVector>
#include <QVector3D>
#include <QVariantList>

class Modifier : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QVariantList colorList READ colorList WRITE setColorList NOTIFY colorListChanged)

public:
    Modifier();
    virtual void apply(class AtomData &atomData) = 0;
    bool enabled() const;
    void setSystem(class System *system);
    Q_INVOKABLE virtual void parseCommand(QString cmd);
    QVariantList colorList() const;

public slots:
    void setEnabled(bool enabled);
    void setColorList(QVariantList colorList);

signals:
    void enabledChanged(bool enabled);
    void colorListChanged(QVariantList colorList);

protected:
    class System *m_system = nullptr;
    bool m_enabled = false;
    QVector<QVector3D> m_colors;

private:
    QVariantList m_colorList;
};

#endif // MODIFIER_H
