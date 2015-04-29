#ifndef GEOMETRYSELECTOR_H
#define GEOMETRYSELECTOR_H

#include <QWidget>
#include <QPushButton>
#include <QTableWidget>

class QEvent;
class QMouseEvent;

class GeometrySelector : public QWidget
{
    Q_OBJECT
public:
    explicit GeometrySelector(QWidget *parent = 0);
    
signals:
    
public slots:
    void onButtonPress();
    
private:
    void fillTableWithSquares();

    QPushButton  m_button;

    int m_nRows;
    int m_nColumns;

    QTableWidget m_table;
};

class HighlightableBox : public QWidget
{
    Q_OBJECT
public:
    explicit HighlightableBox(QWidget *parent = 0);

private:
    void enterEvent(QEvent *pEvent);
    void leaveEvent(QEvent* pEvent);
    void moveEvent(QMoveEvent *pEvent);

};

#endif // GEOMETRYSELECTOR_H
