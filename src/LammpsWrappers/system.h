#ifndef SYSTEM_H
#define SYSTEM_H

#include <QObject>
#include <QVariant>
#include <QVector3D>
#include <mpi.h>
#include <lammps.h>
#include <qvector.h>
#include <domain.h>
#include <QMatrix4x4>
#include <QMatrix3x3>
#include <version.h>
#include <memory>

class System : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVector3D size READ size NOTIFY sizeChanged)
    Q_PROPERTY(QVector3D origin READ origin NOTIFY originChanged)
    Q_PROPERTY(QVector3D cameraPosition READ cameraPosition WRITE setCameraPosition NOTIFY cameraPositionChanged)
    Q_PROPERTY(QVector3D center READ center NOTIFY centerChanged)
    Q_PROPERTY(QMatrix4x4 transformationMatrix READ transformationMatrix NOTIFY transformationMatrixChanged)
    Q_PROPERTY(QMatrix3x3 cellMatrix READ cellMatrix WRITE setCellMatrix NOTIFY cellMatrixChanged)
    Q_PROPERTY(QString boundaryStyle READ boundaryStyle WRITE setBoundaryStyle NOTIFY boundaryStyleChanged)
    Q_PROPERTY(double cpuremain READ cpuremain WRITE setCpuremain NOTIFY cpuremainChanged)
    Q_PROPERTY(bool triclinic READ triclinic WRITE setTriclinic NOTIFY triclinicChanged)
    Q_PROPERTY(int numberOfAtoms READ numberOfAtoms NOTIFY numberOfAtomsChanged)
    Q_PROPERTY(int numberOfAtomTypes READ numberOfAtomTypes NOTIFY numberOfAtomTypesChanged)
    Q_PROPERTY(float volume READ volume NOTIFY volumeChanged)
    Q_PROPERTY(float density READ density WRITE setDensity NOTIFY densityChanged)
    Q_PROPERTY(float simulationTime READ simulationTime NOTIFY simulationTimeChanged)
    Q_PROPERTY(double dt READ dt WRITE setDt NOTIFY dtChanged)
    Q_PROPERTY(int currentTimestep READ currentTimestep NOTIFY currentTimestepChanged)
    Q_PROPERTY(Performance *performance READ performance NOTIFY performanceChanged)
    Q_PROPERTY(Atoms* atoms READ atoms NOTIFY atomsChanged)
    Q_PROPERTY(Regions* regions READ regions NOTIFY regionsChanged)
    Q_PROPERTY(Groups* groups READ groups NOTIFY groupsChanged)
    Q_PROPERTY(Computes* computes READ computes NOTIFY computesChanged)
    Q_PROPERTY(Variables* variables READ variables NOTIFY variablesChanged)
    Q_PROPERTY(Fixes* fixes READ fixes NOTIFY fixesChanged)
    Q_PROPERTY(Units* units READ units NOTIFY unitsChanged)
    Q_PROPERTY(bool isValid READ isValid WRITE setIsValid NOTIFY isValidChanged)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QString lammpsVersion READ lammpsVersion WRITE setLammpsVersion NOTIFY lammpsVersionChanged)
    Q_PROPERTY(QString pairStyle READ pairStyle WRITE setPairStyle NOTIFY pairStyleChanged)
    Q_PROPERTY(bool macAppStore READ macAppStore WRITE setMacAppStore NOTIFY macAppStoreChanged)
    Q_PROPERTY(int numberOfTimesteps READ numberOfTimesteps WRITE setNumberOfTimesteps NOTIFY numberOfTimestepsChanged)
    Q_PROPERTY(int numberOfDangerousNeighborlistBuilds READ numberOfDangerousNeighborlistBuilds WRITE setNumberOfDangerousNeighborlistBuilds NOTIFY numberOfDangerousNeighborlistBuildsChanged)
    Q_PROPERTY(QString lastCommand READ lastCommand WRITE setLastCommand NOTIFY lastCommandChanged)
    Q_PROPERTY(QVariant selection READ selection NOTIFY selectionChanged)
public:
    System(class AtomifySimulator *simulator = nullptr);
    ~System();

    // Properties
    class Atoms* atoms() const;
    class Regions* regions() const;
    class Groups* groups() const;
    class Computes* computes() const;
    class Units* units() const;
    class Fixes* fixes() const;
    class Variables* variables() const;
    QVector3D origin() const;
    QVector3D size() const;
    int numberOfAtoms() const;
    float simulationTime() const;
    int currentTimestep() const;
    int numberOfAtomTypes() const;
    float volume() const;
    QVector3D cameraPosition() const;
    QVector3D center() const;

    // Actions
    void synchronize(class LAMMPSController *lammpsController);
    void synchronizeQML(class LAMMPSController *lammpsController);
    void updateThreadOnDataObjects(QThread *thread);
    void reset();
    bool isValid() const;
    QMatrix4x4 transformationMatrix() const;
    QMatrix3x3 cellMatrix() const;
    QString boundaryStyle() const;
    bool triclinic() const;
    class Performance * performance() const;
    double cpuremain() const;
    double dt() const;
    QString state() const;
    QString lammpsVersion() const;
    bool macAppStore() const;
    QString pairStyle() const;
    int numberOfTimesteps() const;
    int numberOfDangerousNeighborlistBuilds() const;
    QString lastCommand() const;
    float density() const;
    QVector<class SimulatorControl*> simulatorControls() const;
    void setSelection(const QVector<class AtomSelectionData*> &selection);
    QVariant selection() const;

public slots:
    void setIsValid(bool isValid);
    void setCameraPosition(QVector3D cameraPosition);
    void setCellMatrix(QMatrix3x3 cellMatrix);
    void setBoundaryStyle(QString boundaryStyle);
    void setTriclinic(bool triclinic);
    void setCpuremain(double cpuremain);
    void setDt(double dt);
    void setState(QString state);
    void setLammpsVersion(QString lammpsVersion);
    void setMacAppStore(bool macAppStore);
    void setPairStyle(QString pairStyle);
    void setNumberOfTimesteps(int numberOfTimesteps);
    void setNumberOfDangerousNeighborlistBuilds(int numberOfDangerousNeighborlistBuilds);
    void setLastCommand(QString lastCommand);
    void setDensity(float density);

signals:
    void originChanged(QVector3D origin);
    void sizeChanged(QVector3D size);
    void numberOfAtomsChanged(int numberOfAtoms);
    void simulationTimeChanged(float simulationTime);
    void currentTimestepChanged(int currentTimestep);
    void atomsChanged(class Atoms* atoms);
    void regionsChanged(class Regions* regions);
    void groupsChanged(class Groups* groups);
    void unitsChanged(class Units* units);
    void fixesChanged(class Fixes* fixes);
    void computesChanged(class Computes* computes);
    void variablesChanged(class Variables* variables);
    void numberOfAtomTypesChanged(int numberOfAtomTypes);
    void volumeChanged(float volume);
    void isValidChanged(bool isValid);
    void cameraPositionChanged(QVector3D cameraPosition);
    void centerChanged(QVector3D center);
    void transformationMatrixChanged(QMatrix4x4 transformationMatrix);
    void cellMatrixChanged(QMatrix3x3 cellMatrix);
    void boundaryStyleChanged(QString boundaryStyle);
    void triclinicChanged(bool triclinic);
    void performanceChanged(class Performance * performance);
    void cpuremainChanged(double cpuremain);
    void dtChanged(double dt);
    void stateChanged(QString state);
    void lammpsVersionChanged(QString lammpsVersion);
    void macAppStoreChanged(bool macAppStore);
    void pairStyleChanged(QString pairStyle);
    void numberOfTimestepsChanged(int numberOfTimesteps);
    void numberOfDangerousNeighborlistBuildsChanged(int numberOfDangerousNeighborlistBuilds);
    void lastCommandChanged(QString lastCommand);
    void densityChanged(float density);
    void selectionChanged(QVariant selection);

private:
    void updateSelectionData(LAMMPS_NS::LAMMPS *lammps);
    bool makeSureLAMMPSHasMap(LAMMPS_NS::LAMMPS *lammps);
    // TODO these are never changed, consider having them as non-pointer members
    std::unique_ptr<class Atoms> m_atoms;
    std::unique_ptr<class Regions> m_regions;
    std::unique_ptr<class Groups> m_groups;
    std::unique_ptr<class Computes> m_computes;
    std::unique_ptr<class Units> m_units;
    std::unique_ptr<class Fixes> m_fixes;
    std::unique_ptr<class Variables> m_variables;
    std::unique_ptr<class Performance> m_performance;
    QVector3D m_origin;
    QVector3D m_size;
    QVector3D m_cameraPosition;
    int m_numberOfAtoms = 0;
    float m_simulationTime = 0;
    int m_currentTimestep = 0;
    int m_numberOfAtomTypes = 0;
    float m_volume = 0;
    bool m_isValid = false;
    void updateTransformationMatrix(LAMMPS_NS::Domain *domain);
    void updateSizeAndOrigin(LAMMPS_NS::Domain *domain);
    void computeCellMatrix(LAMMPS_NS::Domain *domain);
    void updateBoundaryStyle(LAMMPS_NS::Domain *domain);
    void updateCenter(LAMMPS_NS::Domain *domain);
    QMatrix4x4 m_transformationMatrix;
    QMatrix3x3 m_cellMatrix;
    QString m_boundaryStyle = "None";
    bool m_triclinic = false;
    double m_cpuremain = 0;
    void calculateCPURemain(LAMMPS_NS::LAMMPS *lammps);
    void calculateTimestepsPerSeconds(LAMMPS_NS::LAMMPS *lammps);
    QVector3D m_center;
    double m_dt = 0;
    QString m_state;
    QString m_lammpsVersion;
    bool m_macAppStore = false;
    QString m_pairStyle;
    int m_numberOfTimesteps = 0;
    int m_numberOfDangerousNeighborlistBuilds = 0;
    QString m_lastCommand;
    float m_density = 0;
    QVariant m_selection;
};

#endif // SYSTEM_H
