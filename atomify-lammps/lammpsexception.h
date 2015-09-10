#ifndef LAMMPSERROR_H
#define LAMMPSERROR_H
#include <QString>
#include <exception>
using std::exception;
class LammpsException: public exception
{
private:
    QString m_error;
public:
    LammpsException(QString error) { m_error = error; }
    virtual const char* what() const throw()
    {
        return m_error.toStdString().c_str();
        // return "My exception happened";
    }
};

#endif // LAMMPSERROR_H
