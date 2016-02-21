#ifndef GEOMETRYSELECTOR_H
#define GEOMETRYSELECTOR_H

#include <QWidget>

namespace Ui {
class GeometrySelector;
}

namespace Viewer
{

class GeometrySelector : public QWidget
{
    Q_OBJECT
    
public:
    explicit GeometrySelector(QWidget *parent = 0);
    ~GeometrySelector();
    
    int getRowCount() const;
    int getColumnCount() const;

public slots:
    void onRowCountChanged(int nRows);
    void onColumnCountChanged(int nColumns);

signals:
    void rowCountChanged(int nRows);
    void columnCountChanged(int nColumns);

private:
    Ui::GeometrySelector *ui;
};


} // end of namespace

#endif // GEOMETRYSELECTOR_H
