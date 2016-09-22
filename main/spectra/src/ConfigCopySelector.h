#ifndef CONFIGCOPYSELECTOR_H
#define CONFIGCOPYSELECTOR_H

#include <QWizard>
#include <QButtonGroup>
#include <QString>
#include <QStringList>
#include <QMap>

#include <memory>
#include <utility>
#include <vector>


class QGridLayout;
class QWizardPage;
class QCheckBox;
class QAbstractButton;

class TH1;

namespace Ui {
class ConfigCopySelector;
}

namespace Viewer {

class SpectrumView;
class SpecTclInterface;
class HistogramBundle;

struct ConfigCopyTarget {
    int s_row;
    int s_col;
    TH1*  s_pHist;
};


struct ConfigCopySelection {
    ConfigCopyTarget s_sourceTarget;
    std::vector<ConfigCopyTarget > s_destTargets;
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

    std::map<int, ConfigCopyTarget> m_targetMap;

};

} // end Viewer namespace
#endif // CONFIGCOPYSELECTOR_H
