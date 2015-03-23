#include "HistogramView.h"
#include "ui_HistogramView.h"
#include <ListRequestHandler.h>
#include <TH1.h>
#include <QList>
#include <vector>
#include <HistInfo.h>
#include <HistFactory.h>
#include <HistogramList.h>
#include <QTimer>

HistogramView::HistogramView(QWidget *parent) :
    QDockWidget(tr("Histograms"),parent),
    ui(new Ui::HistogramView),
    m_req(new ListRequestHandler(this))
{
  m_req->setHistogramView(this);
    ui->setupUi(this);

    QTimer::singleShot(2000,this,SLOT(onUpdate()));
    connect(m_req,SIGNAL(parseCompleted(std::vector<SpJs::HistInfo>)),
            this,SLOT(setList(std::vector<SpJs::HistInfo>)));

    connect(ui->histList,SIGNAL(doubleClicked(QModelIndex)),
            this,SLOT(onDoubleClick(QModelIndex)));
}

HistogramView::~HistogramView()
{
    deleteHists();
    delete ui;
}

void HistogramView::onUpdate()
{
    m_req->get();
}

void HistogramView::setList(std::vector<SpJs::HistInfo> names)
{
    SpJs::HistFactory factory;
//    ui->histList->clear();
    auto iter = names.begin();
    auto end = names.end();

    while (iter!=end) {

        SpJs::HistInfo& info = (*iter);
        QString name = QString::fromStdString(info.s_name);
        if (!histExists(name)) {

            // this should  get a unique ptr, we want to get the raw pointer
            // and strip the unique_ptr of its ownership (i.e. release())
            auto upHist = factory.create(info);
            TH1* pHist = upHist.get();
            upHist.release();
            HistogramList::getInstance()->addHist(*pHist);

            QVariant var = QVariant::fromValue((void*)(pHist));

            QString name = QString::fromStdString((*iter).s_name);
            auto item = new QListWidgetItem(name, ui->histList, QListWidgetItem::UserType);
            item->setData(Qt::UserRole,var);

            QSize geo = ui->histList->size();
            ui->histList->insertItem(geo.height(), item);

        }

      ++iter;
    }

    QTimer::singleShot(2000,this,SLOT(onUpdate()));
}

void HistogramView::onDoubleClick(QModelIndex index)
{
    TH1* hist = reinterpret_cast<TH1*>(index.data(Qt::UserRole).value<void*>());
    emit histSelected(*hist);
}


bool HistogramView::histExists(const QString& name)
{
    size_t nRows = ui->histList->count();
    for(size_t  entry=0; entry<nRows; ++entry) {
        QListWidgetItem* item = ui->histList->item(entry);
        if (item->text() == name) {
            return true;
        }
    }
    return false;
}

void HistogramView::deleteHists()
{
    size_t nEntries = ui->histList->count();
    for(size_t  entry=0; entry<nEntries; ++entry) {
        QListWidgetItem* item = ui->histList->item(entry);
        TH1* hist = (TH1*)(item->data(Qt::UserRole).value<void*>());
        delete hist;
    }
}

