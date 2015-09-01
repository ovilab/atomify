#ifndef COMPUTE_H
#define COMPUTE_H

#include <QObject>
#include <QPair>
#include <QVector>

class MySimulator; class LAMMPSController;
class CPCompute : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString identifier READ identifier WRITE setIdentifier NOTIFY identifierChanged)
    Q_PROPERTY(QString command READ command WRITE setCommand NOTIFY commandChanged)
    Q_PROPERTY(MySimulator* simulator READ simulator WRITE setSimulator NOTIFY simulatorChanged)
    Q_PROPERTY(QStringList dependencies READ dependencies WRITE setDependencies NOTIFY dependenciesChanged)
    Q_PROPERTY(double time READ time)
    Q_PROPERTY(double value READ value NOTIFY valueChanged)
private:
    QString m_identifier;
    QString m_command;
    MySimulator* m_simulator = NULL;
    QStringList m_dependencies;
    double m_value = 0;
    double m_time = 0;

public:
    explicit CPCompute(QObject *parent = 0);

    QString identifier() const;
    QString command() const;
    MySimulator* simulator() const;
    QStringList dependencies() const;

    double value() const;
    double time() const;

signals:
    void identifierChanged(QString identifier);
    void commandChanged(QString command);
    void simulatorChanged(MySimulator* simulator);
    void dependenciesChanged(QStringList dependencies);
    void valueChanged(double value);

public slots:
    void setIdentifier(QString identifier);
    void setCommand(QString command);
    void setSimulator(MySimulator* simulator);
    void setDependencies(QStringList dependencies);
    void setValue(QPair<double,double> timeAndValue);
};
#endif // COMPUTE_H
