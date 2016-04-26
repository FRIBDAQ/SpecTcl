#ifndef TABFROMWINFILECOMPOSITOR_H
#define TABFROMWINFILECOMPOSITOR_H

#include <memory>

class QString;

class win_attributed;

namespace Viewer
{

class QRootCanvas;
class SpecTclInterface;
class TabWorkspace;

class TabFromWinFileCompositor
{
public:
    TabFromWinFileCompositor(std::shared_ptr<SpecTclInterface> pSpecTcl);

    void compose(TabWorkspace& rWorkSpace, const QString& fileName);

    void setUpCanvas(QRootCanvas &rCanvas, win_attributed& rAttributes );

private:
    std::shared_ptr<SpecTclInterface> m_pSpecTcl;
};

} // end Viewer namespace

#endif // TABFROMWINFILECOMPOSITOR_H
