#ifndef FIXES_H
#define FIXES_H

#include <QObject>
#include <QVariant>

class Fixes : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(QVariantList fixIndents READ fixIndents WRITE setFixIndents NOTIFY fixIndentsChanged)
    Q_PROPERTY(CPFixIndent* activeFixIndent READ activeFixIndent WRITE setActiveFixIndent NOTIFY activeFixIndentChanged)
    Q_PROPERTY(int numFixIndents READ numFixIndents WRITE setNumFixIndents NOTIFY numFixIndentsChanged)
public:
    explicit Fixes(QObject *parent = 0);

    QVariant model() const;
    void reset();
    void synchronize(class LAMMPSController *lammpsController);
    int count() const;
    QVector<class SimulatorControl*> simulatorControls();

    void synchronizeQML(LAMMPSController *lammpsController);
    void updateThreadOnDataObjects(QThread *thread);
    class CPFixIndent* activeFixIndent() const;
    QVariantList fixIndents() const;

    int numFixIndents() const
    {
        return m_numFixIndents;
    }

signals:
    void modelChanged(QVariant model);
    void countChanged(int count);
    void activeFixIndentChanged(class CPFixIndent* activeFixIndent);
    void fixIndentsChanged(QVariantList fixIndents);
    void numFixIndentsChanged(int numFixIndents);

public slots:
    void setModel(QVariant model);
    void setCount(int count);
    void setActiveFixIndent(class CPFixIndent* activeFixIndent);
    void setFixIndents(QVariantList fixIndents);

    void setNumFixIndents(int numFixIndents)
    {
        if (m_numFixIndents == numFixIndents)
            return;

        m_numFixIndents = numFixIndents;
        emit numFixIndentsChanged(m_numFixIndents);
    }

private:
    int m_count = 0;
    QVariant m_model;
    QList<QObject *> m_data;
    QMap<QString, QObject *> m_dataMap;
    class CPFixIndent* m_activeFixIndent = nullptr;
    void add(QString identifier, class LAMMPSController *lammpsController);
    void remove(QString identifier);
    bool addOrRemove(LAMMPSController *lammpsController);
    QVariantList m_fixIndents;
    int m_numFixIndents = 0;
};

#endif // FIXES_H
