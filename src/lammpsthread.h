#ifndef LAMMPSTHREAD_H
#define LAMMPSTHREAD_H

#include <QThread>

namespace atomify {

class LAMMPSThread : public QThread
{
    Q_OBJECT
public:
    explicit LAMMPSThread(QObject *parent = nullptr);

    // QThread interface
protected:
    void run() override;
    void callback(class TMPLAMMPS *lmp);
};

}
#endif // LAMMPSTHREAD_H
