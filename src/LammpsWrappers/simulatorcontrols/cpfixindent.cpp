#include "cpfixindent.h"
#include "../../lammpscontroller.h"
#include <input.h>
#include <domain.h>
#include <fix_indent.h>

CPFixIndent::CPFixIndent(QQuickItem *parent) : CPFix(parent)
{
    setInteractive(true);
    setQmlFileName( QUrl("SimulatorControlItems/FixIndentItem.qml") );
}

CPFixIndent::~CPFixIndent()
{

}


void CPFixIndent::copyData(LAMMPSController *lammpsController)
{
    enum{NONE,SPHERE,CYLINDER,PLANE};
    LAMMPS_NS::FixIndent *lmp_fix = dynamic_cast<LAMMPS_NS::FixIndent *>(lammpsController->findFixByIdentifier(identifier()));
    if(lmp_fix == nullptr) return;
    int dim;
    int    *istyle = static_cast<int*>(lmp_fix->extract("istyle", dim));
    int    *cdim   = static_cast<int*>(lmp_fix->extract("cdim", dim));
    int    *xvar = static_cast<int*>(lmp_fix->extract("xvar", dim));
    int    *yvar   = static_cast<int*>(lmp_fix->extract("yvar", dim));
    int    *zvar   = static_cast<int*>(lmp_fix->extract("zvar", dim));
    int    *rvar   = static_cast<int*>(lmp_fix->extract("rvar", dim));
    int    *pvar   = static_cast<int*>(lmp_fix->extract("pvar", dim));
    double *xvalue = static_cast<double*>(lmp_fix->extract("xvalue", dim));
    double *yvalue = static_cast<double*>(lmp_fix->extract("yvalue", dim));
    double *zvalue = static_cast<double*>(lmp_fix->extract("zvalue", dim));
    double *rvalue = static_cast<double*>(lmp_fix->extract("rvalue", dim));
    double *pvalue = static_cast<double*>(lmp_fix->extract("pvalue", dim));
    char   *xstr   = static_cast<char*>(lmp_fix->extract("xstr", dim));
    char   *ystr   = static_cast<char*>(lmp_fix->extract("ystr", dim));
    char   *zstr   = static_cast<char*>(lmp_fix->extract("zstr", dim));
    char   *rstr   = static_cast<char*>(lmp_fix->extract("rstr", dim));
    char   *pstr   = static_cast<char*>(lmp_fix->extract("pstr", dim));
    if (*istyle == SPHERE) {
        setType("sphere");
        // ctr = current indenter center
        // remap into periodic box

        double ctr[3];
        if (xstr) ctr[0] = lammpsController->lammps()->input->variable->compute_equal(*xvar);
        else ctr[0] = *xvalue;
        if (ystr) ctr[1] = lammpsController->lammps()->input->variable->compute_equal(*yvar);
        else ctr[1] = *yvalue;
        if (zstr) ctr[2] = lammpsController->lammps()->input->variable->compute_equal(*zvar);
        else ctr[2] = *zvalue;
        lammpsController->lammps()->domain->remap(ctr);
        double radius;
        if (rstr) radius = lammpsController->lammps()->input->variable->compute_equal(*rvar);
        else radius = *rvalue;
        setPosition(QVector3D(ctr[0], ctr[1], ctr[2]));
        setRadius(radius);
        setRotation(QQuaternion(0,0,0,0));
    } else if (*istyle == CYLINDER) {
        setType("cylinder");
        // ctr = current indenter axis
        // remap into periodic box
        // 3rd coord is just near box for remap(), since isn't used

        double ctr[3];
        if (*cdim == 0) {
            ctr[0] = lammpsController->lammps()->domain->boxlo[0];
            if (ystr) ctr[1] = lammpsController->lammps()->input->variable->compute_equal(*yvar);
            else ctr[1] = *yvalue;
            if (zstr) ctr[2] = lammpsController->lammps()->input->variable->compute_equal(*zvar);
            else ctr[2] = *zvalue;
        } else if (*cdim == 1) {
            if (xstr) ctr[0] = lammpsController->lammps()->input->variable->compute_equal(*xvar);
            else ctr[0] = *xvalue;
            ctr[1] = lammpsController->lammps()->domain->boxlo[1];
            if (zstr) ctr[2] = lammpsController->lammps()->input->variable->compute_equal(*zvar);
            else ctr[2] = *zvalue;
        } else {
            if (xstr) ctr[0] = lammpsController->lammps()->input->variable->compute_equal(*xvar);
            else ctr[0] = *xvalue;
            if (ystr) ctr[1] = lammpsController->lammps()->input->variable->compute_equal(*yvar);
            else ctr[1] = *yvalue;
            ctr[2] = lammpsController->lammps()->domain->boxlo[2];
        }
        lammpsController->lammps()->domain->remap(ctr);

        double radius;
        if (rstr) radius = lammpsController->lammps()->input->variable->compute_equal(*rvar);
        else radius = *rvalue;
        if (*cdim == 0) {
            setPosition(QVector3D(0, ctr[1], ctr[2]));
        } else if (*cdim == 1) {
            setPosition(QVector3D(ctr[0], 0, ctr[2]));
        } else {
            setPosition(QVector3D(ctr[0], ctr[1], 0));
        }
        setRadius(radius);
        setDimension(*cdim);

        if(*cdim==0) {
            setRotation(QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), 90));
        } else if(*cdim==1) {
            setRotation(QQuaternion(0,0,0,0));
        } else {
            setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), -90));
        }

    } else if (*istyle == PLANE) {
        setType("plane");
        // plane = current plane position
        double plane;
        if (pstr) plane = lammpsController->lammps()->input->variable->compute_equal(*pvar);
        else plane = *pvalue;
        setDimension(*cdim);
        setPosition(QVector3D(plane, plane, plane));

        if(*cdim==0) {
            setRotation(QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), 90));
        } else if(*cdim==1) {
            setRotation(QQuaternion(0,0,0,0));
        } else {
            setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), -90));
        }
    } else {
        setType("none");
    }
}

QString CPFixIndent::type() const
{
    return m_type;
}

QVector3D CPFixIndent::position() const
{
    return m_position;
}

int CPFixIndent::dimension() const
{
    return m_dimension;
}

qreal CPFixIndent::radius() const
{
    return m_radius;
}

QQuaternion CPFixIndent::rotation() const
{
    return m_rotation;
}

void CPFixIndent::setType(QString type)
{
    if (m_type == type)
        return;

    m_type = type;
    emit typeChanged(m_type);
}

void CPFixIndent::setPosition(QVector3D position)
{
    if (m_position == position)
        return;

    m_position = position;
    emit positionChanged(m_position);
}

void CPFixIndent::setDimension(int dimension)
{
    if (m_dimension == dimension)
        return;

    m_dimension = dimension;
    emit dimensionChanged(m_dimension);
}

void CPFixIndent::setRadius(qreal radius)
{
    if (qFuzzyCompare(m_radius, radius))
        return;

    m_radius = radius;
    emit radiusChanged(m_radius);
}

void CPFixIndent::setRotation(QQuaternion rotation)
{
    if (m_rotation == rotation)
        return;

    m_rotation = rotation;
    emit rotationChanged(m_rotation);
}
