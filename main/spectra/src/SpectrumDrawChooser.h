#ifndef SPECTRUMDRAWCHOOSER_H
#define SPECTRUMDRAWCHOOSER_H

#include "HistogramList.h"
#include "QHistInfo.h"
#include "ui_HistogramView.h"

#include <QWidget>

#include <QModelIndex>

#include <vector>
#include <memory>


namespace Ui {
class SpectrumDrawChooser;
}

class QListWidgetItem;
class TH1;


namespace Viewer
{

class SpecTclInterface;
class ListRequestHandler;
class GuardedHist;

/*! \brief A widget that contains the list of histograms
 *
 * The HistogramView provides the user a graphical interaction point
 * to view and select which histogram the user wants to draw. This only
 * maintains a reference to the histograms that are owned by the HistogramList.
 * This therefore has a synchronization mechanism to ensure that the displayed
 * names are what are available.
 *
 * This also implements the requisite method to be wrapped in a
 * GenericSpecTclInterfaceObserver.
 */
class SpectrumDrawChooser : public QWidget
{
    Q_OBJECT

public:

    explicit SpectrumDrawChooser(std::shared_ptr<SpecTclInterface> pSpecTcl,

                           QWidget *parent = 0);
    ~SpectrumDrawChooser();

    /*!
     * \brief Pass in a new SpecTclInterface
     *
     * \param pSpecTcl
     */
    void setSpecTclInterface(std::shared_ptr<SpecTclInterface> pSpecTcl);

    //////////////////////////////////////////////////////////////////////////

    /*!
     * \brief Synchronize the view to the master histogram list
     *
     * \param pHistList    pointer to the histogram list
     */
    void synchronize(HistogramList* pHistList);

    void appendEntry(const QString& name, HistogramList::iterator it);
    void updateEntry(HistogramList::iterator it, const QString& name);
    void removeStaleEntries(HistogramList *pHistList);
signals:
    void histSelected(HistogramBundle* hist);

    //////////////////////////////////////////////////////////////////////////
public slots:
    void setList(std::vector<SpJs::HistInfo> list);
    void onDrawClicked();

    /*!
     * \brief
     * \param index
     */
    void onDoubleClick(QModelIndex index);

    /*!
     * \brief Synchronize to the master histogram list
     */
    void onHistogramListChanged();

    //////////////////////////////////////////////////////////////////////////
private:
    /*!
     * \brief histExists
     * \param name
     * \return
     */
    bool histExists(const QString& name);
    void deleteHists();
    void setIcon(QListWidgetItem* pItem);

    /*!
     * \brief Binary search the ListWidget for a
     * \param min    lower bound index
     * \param max    upper bound index
     * \param name   name
     * \return
     */
    int binarySearch(int min, int max, const QString& name);

private:
    std::unique_ptr<Ui::SpectrumDrawChooser> ui;
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
};

} // end of namespace


#endif // SPECTRUMDRAWCHOOSER_H
