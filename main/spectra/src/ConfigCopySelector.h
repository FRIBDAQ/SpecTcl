#ifndef CONFIGCOPYSELECTOR_H
#define CONFIGCOPYSELECTOR_H

#include <QWizard>
#include <QButtonGroup>
#include <QString>
#include <QStringList>
#include <QMap>

#include <memory>


class QGridLayout;
class QWizardPage;
class QCheckBox;
class QAbstractButton;

namespace Ui {
class ConfigCopySelector;
}

namespace Viewer {

class SpectrumView;
class SpecTclInterface;
class HistogramBundle;

struct ConfigCopySelection {
	QString 	s_sourceHist;
	QStringList s_destinationHists;
	bool		s_copyXAxis;
	bool 		s_copyYAxis;
	bool		s_copyDrawOption;
};

class ConfigCopySelector : public QWizard
{
    Q_OBJECT
    
public:
    explicit ConfigCopySelector(SpectrumView& rView,
                                std::shared_ptr<SpecTclInterface> pSpecTcl,
                                QWidget *parent = 0);

    ~ConfigCopySelector();

    ConfigCopySelection getSelection() const;

    bool compatibleHists(HistogramBundle* pSourceBundle, HistogramBundle* pDestBundle);

protected:
    void initializePage(int id);

public slots:
    virtual void accept();
    void setDestinationsChecked(bool checked);
    void setCheckedOptions(bool checked);

private:
    QWizardPage* createSelectSourcePage();
    QWizardPage* createSelectDestinationPage();
    QWizardPage* createSelectConfigOptionsPage();
    QGridLayout* createDummyDisplay(QButtonGroup& group, bool autoExclusive);

    void showAllDestinationButtons();

    QMap<QAbstractButton*,bool> getDestinationCompatibility();
private:
    SpectrumView*   m_pView;
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
    QCheckBox* 		m_pXAxisOption;
    QCheckBox* 		m_pYAxisOption;
    QCheckBox* 		m_pDrawOption;

    QButtonGroup    m_sourceGroup;
    QButtonGroup    m_destinationGroup;

};

} // end Viewer namespace
#endif // CONFIGCOPYSELECTOR_H
