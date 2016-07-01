#ifndef PRINTINGSYSTEM_H
#define PRINTINGSYSTEM_H

#include <QString>
#include <QTextStream>

namespace Viewer {


/*! \brief Basic Interface to Printing Subsystem
 *
 * Because ROOT doesn't play well with the Qt printing subsystem, we
 * have to strike out on our own. This printing subsystem provides some
 * standard utilities that are useful when printing, like querying the
 * available printers and keeping track of a default printer.
 *
 * The assumption here is that we are dealing with a Linux system that uses
 * CUPS for its printing platform. This should not be much of a problem.
 *
 */
class PrintingSystem {
private:
    static PrintingSystem* m_pInstance;
    QString         m_defaultPrinter;

    PrintingSystem();
    PrintingSystem(const PrintingSystem&);

public:
    static PrintingSystem& instance();


    void        setDefaultPrinter(const QString& printer);
    QString     getDefaultPrinter();

    /*! \returns list of printers to print to */
    QStringList getAvailablePrinters();

private:
    QStringList extractPrintersFromStream(QTextStream &stream);
    void        askSystemForAvailablePrinters();

};

} // end Viewer
#endif // PRINTINGSYSTEM_H
