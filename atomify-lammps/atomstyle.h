#ifndef ATOMSKIN_H
#define ATOMSKIN_H
#include <QDebug>
#include <QObject>
#include <QColor>

class AtomStyleData : public QObject {
    Q_OBJECT
    Q_PROPERTY(double scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QColor color READ color  WRITE setColor NOTIFY colorChanged)

    double m_scale;
    QColor m_color;

public:
    AtomStyleData(double scale, QColor color);
    double scale() const;
    QColor color() const;

public slots:
    void setScale(double scale);
    void setColor(QColor color);

signals:
    void scaleChanged(double scale);
    void colorChanged(QColor color);
};

class AtomStyle : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    QVariant m_model;
    QList<QObject*> m_data;

public:
    explicit AtomStyle(QObject *parent = 0);
    ~AtomStyle();
    QVariant model() const;
    void setColorsAndScales(QVector<QColor> &colors, QVector<float> &scales, QVector<int> &atomTypes);
    void setData(QList<QObject*> data);
    QList<QObject*> data();

signals:
    void modelChanged(QVariant model);

public slots:
    void setModelData(const int index, const QString &key, const QVariant &value);
    void setModel(QVariant model);
    void add();
};

#endif // ATOMSKIN_H
