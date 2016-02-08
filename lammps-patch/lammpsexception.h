#ifndef LAMMPSEXCEPTION_H
#define LAMMPSEXCEPTION_H
#include <string>
#include <exception>

class LammpsException: public std::exception
{
public:
    LammpsException();
    ~LammpsException() throw();

    LammpsException(const char* file, const char* str, int line);
    std::string file();
    std::string error();
    int line();
    bool isReported();
    void setIsReported(bool isReported);
private:
    std::string m_file;
    int m_line;
    std::string m_error;
    bool m_isReported;
};

inline LammpsException::LammpsException() {
    m_isReported = true;
}

inline LammpsException::~LammpsException() throw()
{
}

inline LammpsException::LammpsException(const char *file, const char *str, int line) {
    if(str != 0) {
        m_error = std::string(str);
    }
    if(file != 0) {
        m_file = std::string(file);
    }
    m_line = line;
    m_isReported = false;
}

inline std::string LammpsException::file() { return m_file; }

inline std::string LammpsException::error() { return m_error; }

inline int LammpsException::line() { return m_line; }

inline bool LammpsException::isReported() { return m_isReported; }

inline void LammpsException::setIsReported(bool isReported) { m_isReported = isReported;}

#endif // LAMMPSEXCEPTION_H
