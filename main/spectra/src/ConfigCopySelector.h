#ifndef CONFIGCOPYSELECTOR_H
#define CONFIGCOPYSELECTOR_H

#include <QWizard>
#include <QButtonGroup>
#include <QString>
#include <QStringList>

class QGridLayout;
class QWizardPage;
class QCheckBox;

namespace Ui {
class ConfigCopySelector;
}

namespace Viewer {

class SpectrumView;


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
                                QWidget *parent = 0);

    ~ConfigCopySelector();

    ConfigCopySelection getSelection() const;
    
public slots:
    virtual void accept();
    void setDestinationsChecked(bool checked);
    void setCheckedOptions(bool checked);

private:
    QWizardPage* createSelectSourcePage();
    QWizardPage* createSelectDestinationPage();
    QWizardPage* createSelectConfigOptionsPage();
    QGridLayout* createDummyDisplay(QButtonGroup& group, bool autoExclusive);

private:
    SpectrumView*   m_pView;
    QCheckBox* 		m_pXAxisOption;
    QCheckBox* 		m_pYAxisOption;
    QCheckBox* 		m_pDrawOption;

    QButtonGroup    m_sourceGroup;
    QButtonGroup    m_destinationGroup;

};

} // end Viewer namespace
#endif // CONFIGCOPYSELECTOR_H
