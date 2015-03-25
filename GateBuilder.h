#ifndef GATEBUILDER_H
#define GATEBUILDER_H

#include <QDialog>
#include <QList>
#include <QPair>
#include <QString>

class TCutG;
class TPad;

class GateBuilder : public QDialog
{
    Q_OBJECT

public:
    GateBuilder(QObject* parent=0);

public slots:
    virtual void accept();

    void newPoint(TPad* pad);
    void complete();
    void nameChanged(const QString& name);

private:
    QString m_name;
    std::vector<double> m_xPoints;
    std::vector<double> m_yPoints;
};

#endif // GATEBUILDER_H
