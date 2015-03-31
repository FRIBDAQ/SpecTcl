#ifndef GSLICE_H
#define GSLICE_H

#include <QObject>
#include <QString>
#include <QMetaType>

class TLine;
class TFrame;
class QRootCanvas;

class GSlice : public QObject
{
    Q_OBJECT
public:
    explicit GSlice(QObject *parent = 0,
                    const QString& param = QString(),
                    double xLow = 0,
                    double xHigh = 0,
                    QRootCanvas* pCanvas=nullptr);

    virtual ~GSlice();

    GSlice& operator=(const GSlice& rhs);
    
    double getXLow() const;
    double getXHigh() const;

    TLine* getXLowLine() { return m_pLow; }
    TLine* getXHighLine() { return m_pHigh; }
    QString getName() const { return m_name; }
    QString getParameter() const { return m_parameter; }

public slots:
    void draw(QRootCanvas* canvas);
    void draw();
    void setXLow(double x);
    void setXHigh(double x);
    void frameChanged();
    void frameChanged(TFrame* frame);
    void nameChanged(const QString& name);

private:
    QString m_name;
    TLine* m_pLow;
    TLine* m_pHigh;
    QRootCanvas* m_pCanvas;
    QString m_parameter;
};


#endif // GSLICE_H
