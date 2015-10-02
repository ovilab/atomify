#ifndef ATOMSKIN_H
#define ATOMSKIN_H
#include <QDebug>
#include <QObject>
#include <QColor>

class AtomStyleData : public QObject {
    Q_OBJECT
    Q_PROPERTY(double scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QColor color READ color  WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)

    bool m_visible = true;
    double m_scale = 1.0;
    QColor m_color;

public:
    AtomStyleData(double scale, QColor color);
    double scale() const;
    QColor color() const;
    bool visible() const;

public slots:
    void setScale(double scale);
    void setColor(QColor color);
    void setVisible(bool visible);

signals:
    void scaleChanged(double scale);
    void colorChanged(QColor color);
    void visibleChanged(bool visible);
};

class AtomStyle : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(bool dirty READ dirty WRITE setDirty NOTIFY dirtyChanged)
    QVariant m_model;
    QList<QObject*> m_data;
    bool m_dirty = false;

public:
    explicit AtomStyle(QObject *parent = 0);
    ~AtomStyle();
    QVariant model() const;
    void setColorsAndScales(QVector<QColor> &colors, QVector<float> &scales, QVector<int> &atomTypes);
    void setData(QList<QObject*> data);
    QList<QObject*> data();
    bool dirty() const;

signals:
    void modelChanged(QVariant model);
    void dirtyChanged(bool dirty);

public slots:
    void setModelData(const int index, const QString &key, const QVariant &value);
    void setModel(QVariant model);
    void add();
    void remove(const int index);
    void setMinimumSize(int minimumSize);
    void setDirty(bool dirty);
};

#endif // ATOMSKIN_H
