#ifndef GGATE_H
#define GGATE_H

#include <QObject>

#include <GateInfo.h>

#include <memory>
#include <vector>
#include <utility>

#include <TCutG.h>


class GGate : public QObject
{
    Q_OBJECT
public:
    explicit GGate(const QString& name,
                   const SpJs::GateInfo2D& info,
                   QObject* parent = nullptr);
    
    GGate& operator=(const GGate& rhs);

    QString getName() const;
    void setName(const QString& name);

    QString getParameterX() const;
    QString getParameterY() const;

    std::vector<std::pair<double, double> > getPoints() const;

    void setInfo(const SpJs::GateInfo2D& info);

    void appendPoint(double x, double y);
    void popBackPoint();

    TCutG* getGraphicObject() { return m_pCut.get(); }

    void draw();

    SpJs::GateType getType() const;
    void setType(SpJs::GateType type);

signals:
    
public slots:
    void onPointChanged(int index, double x, double y);
    void onNameChanged(const QString& name);

private:
    QString m_name;
    std::unique_ptr<TCutG> m_pCut;
    std::unique_ptr<SpJs::GateInfo2D> m_info;

};

#endif // GGATE_H
