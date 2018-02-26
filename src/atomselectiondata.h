#ifndef ATOMSELECTION_H
#define ATOMSELECTION_H

#include <QObject>
#include <QVector3D>
#include <QVariantMap>

class AtomSelectionData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap properties READ properties NOTIFY propertiesChanged)
    Q_PROPERTY(QVector3D position READ position NOTIFY positionChanged)
    Q_PROPERTY(QVector3D velocity READ velocity NOTIFY velocityChanged)
    Q_PROPERTY(QVector3D force READ force NOTIFY forceChanged)
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int type READ type WRITE setType NOTIFY typeChanged)

public:
    explicit AtomSelectionData(QObject *parent = nullptr);
    QVariantMap properties() const;
    void setProperties(const QVariantMap &properties);
    int id() const;
    QVector3D position() const;
    QVector3D velocity() const;
    QVector3D force() const;
    void setPosition(QVector3D position);
    void setVelocity(QVector3D velocity);
    void setForce(QVector3D force);
    int type() const;

signals:
    void propertiesChanged(QVariantMap properties);
    void idChanged(int id);
    void positionChanged(QVector3D position);
    void velocityChanged(QVector3D velocity);
    void forceChanged(QVector3D force);
    void typeChanged(int type);

public slots:
    void setId(int id);
    void setType(int type);

private:
    int m_id;
    int m_type;
    QVector3D m_position;
    QVector3D m_velocity;
    QVector3D m_force;
    QVariantMap m_properties;
};

#endif // ATOMSELECTION_H
