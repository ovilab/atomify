#ifndef LAMMPSERROR_H
#define LAMMPSERROR_H
#include <QString>
#include <QDebug>
#include <exception>
using std::exception;
class LammpsException: public exception
{
private:
    QString m_file;
    int m_line;
    QString m_error;
    QString m_fullError;
    bool m_isReported = false;
public:
    LammpsException() { }

    LammpsException(QString file, QString error, int line) {
        m_error = error;
        m_file = file;
        m_line = line;
        m_fullError = QString("ERROR: %1 (%2:%3)").arg(m_error, m_file).arg(m_line);
    }
    QString file() { return m_file; }
    QString error() { return m_error; }
    int line() { return m_line; }
    virtual const char* what() const throw() { return m_fullError.toStdString().c_str(); }
    bool isReported() { return m_isReported; }
    void setIsReported(bool isReported) { m_isReported = isReported;}
};

#endif // LAMMPSERROR_H




