#ifndef LAMMPSEXCEPTION_H
#define LAMMPSEXCEPTION_H
#include <string>
#include <exception>
using std::exception; using std::string;

class LammpsException: public exception
{
private:
    string m_file;
    int m_line;
    string m_error;
    bool m_isReported;
public:
    LammpsException() { m_isReported = true; }
    ~LammpsException() throw(){};

    LammpsException(string file, string error, int line) {
        m_error = error;
        m_file = file;
        m_line = line;
        m_isReported = false;
    }
    string file() { return m_file; }
    string error() { return m_error; }
    int line() { return m_line; }
    bool isReported() { return m_isReported; }
    void setIsReported(bool isReported) { m_isReported = isReported;}
};

#endif // LAMMPSEXCEPTION_H




