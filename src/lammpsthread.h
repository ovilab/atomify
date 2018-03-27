#ifndef LAMMPSTHREAD_H
#define LAMMPSTHREAD_H

#include <QThread>
#include <QMutex>

namespace atomify {

struct LAMMPSData {
    int timestep = 0;
};

class LAMMPSThread : public QThread
{
    Q_OBJECT
public:
    explicit LAMMPSThread(QObject *parent = nullptr);
    LAMMPSData data();
    bool dataDirty() const;

protected:
    void run() override;
    void callback(class TMPLAMMPS *lmp);
private:
    mutable QMutex m_mutex;
    LAMMPSData m_data;
    bool m_dataDirty;
};

}
#endif // LAMMPSTHREAD_H
