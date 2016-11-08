#ifndef SCRIPT_H
#define SCRIPT_H
#include <QString>
#include <QObject>
#include <QVector>
#include <QMap>

class Script : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(QString script READ script WRITE setScript NOTIFY scriptChanged)
    Q_PROPERTY(int currentLine READ currentLine WRITE setCurrentLine NOTIFY currentLineChanged)
public:
    explicit Script(QObject *parent = 0);
    QString fileName() const;
    QString script() const;
    int currentLine() const;
    bool hasNextLine() const;
    QString getNextLine();

signals:
    void fileNameChanged(QString fileName);
    void scriptChanged(QString script);
    void currentLineChanged(int currentLine);

public slots:
    void setFileName(QString fileName);
    void setScript(QString script);
    void setCurrentLine(int currentLine);

private:
    void readLines();
    int m_currentLine;
    QString m_fileName;
    QString m_script;
    QStringList m_lines;
    QMap<QString, int> m_labels; // Maps labels to line numbers
};

#endif // SCRIPT_H
