
#ifndef QTLINE_H
#define QTLINE_H

#include <TLine.h>

#include <QObject>

#include <iosfwd>

class QTLine : public QObject, virtual public TLine
{
  Q_OBJECT 

  public:
    QTLine();
    QTLine(double x1, double y1, double x2, double y2);

    QTLine& operator=(const QTLine& rhs);

    void Paint(Option_t* opts = "");

    void setEditable(bool enable);
    bool isEditable() const;

signals:
    void valuesChanged(double x1, double y1, double x2, double y2);

  private:
    bool m_editable;
    double m_lastX1;
    double m_lastY1;
    double m_lastX2;
    double m_lastY2;
};

extern std::ostream& operator<<(std::ostream& stream, const QTLine& line);

#endif
