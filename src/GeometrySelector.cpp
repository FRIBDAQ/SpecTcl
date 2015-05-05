#include "GeometrySelector.h"
#include "ui_GeometrySelector.h"


namespace Viewer
{

GeometrySelector::GeometrySelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeometrySelector)
{
    ui->setupUi(this);
}

GeometrySelector::~GeometrySelector()
{
    delete ui;
}


} // end of namespace
