#ifndef CONFIGCOPYSELECTOR_H
#define CONFIGCOPYSELECTOR_H

#include <QWizard>

#include <memory>

class QGridLayout;
class QWizardPage;
class QCheckBox;

namespace Ui {
class ConfigCopySelector;
}

namespace Viewer {

class SpectrumView;

class ConfigCopySelector : public QWizard
{
    Q_OBJECT
    
public:
    explicit ConfigCopySelector(SpectrumView& rView,
                                QWidget *parent = 0);
    ~ConfigCopySelector();
    
private:
    QWizardPage* createSelectSourcePage();
    QWizardPage* createSelectDestinationPage();
    QWizardPage* createSelectConfigOptionsPage();
    QGridLayout* createDummyDisplay();

private:
    SpectrumView* m_pView;
    QCheckBox* 		m_pXAxisOption;
    QCheckBox* 		m_pYAxisOption;
    QCheckBox* 		m_pDrawOption;
};

} // end Viewer namespace
#endif // CONFIGCOPYSELECTOR_H
