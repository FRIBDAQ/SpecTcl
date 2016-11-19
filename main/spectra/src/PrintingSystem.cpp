#include "PrintingSystem.h"

#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QString>

#include <stdexcept>
#include <string>

namespace Viewer {

//
//
PrintingSystem* PrintingSystem::m_pInstance = nullptr;

//
//
PrintingSystem::PrintingSystem() : m_defaultPrinter("lp")
{}

//
//
PrintingSystem& PrintingSystem::instance()
{
    if (m_pInstance == nullptr) {
        m_pInstance == new PrintingSystem;
    }
    return *m_pInstance;
}

//
//
QString PrintingSystem::getDefaultPrinter()
{
    return m_defaultPrinter;
}

//
//
void PrintingSystem::setDefaultPrinter(const QString &printer)
{
    QStringList printers = getAvailablePrinters();
    if (! printers.contains(printer) ) {
        QString msg("PrintingSystem::setDefaultPrinter() ");
        msg += "Cannot set default printer to one that does not exist.";
        throw std::runtime_error(msg.toStdString());
    } else {
        m_defaultPrinter = printer;
    }
}

/*! Extract printers from text stream
 *
 * This provides indirect access to a set of printers in a stream. The stream
 * is really just a way to test. During production, the stream provides access to a
 * file. During testing, the stream provides access to a QString. It is expected
 * that the printer names are separated by newline characters and there is one
 * printer per line.
 *
 * \param   stream
 */
QStringList PrintingSystem::extractPrintersFromStream(QTextStream& stream)
{
    QStringList printers;

    while (!stream.atEnd()) {
        QString printerName = stream.readLine();

        printers.push_back(printerName);
    }

    return printers;
}

void PrintingSystem::askSystemForAvailablePrinters()
{
    // Create a file that contains all of the printers we know about
    // Here we just take the first column of the output returned to us by the lpstat -a
    // command.
    std::system("lpstat -a | awk '{print $1}' > .__temp_printers.txt");
}

/*!
 * \brief PrintingSystem::getAvailablePrinters
 *
 * This queries the CUPS system using lpstat -a and then parses the response
 * into a QStringList.
 *
 * \return list of printers to print to
 */
QStringList PrintingSystem::getAvailablePrinters()
{
    QStringList printers;

    askSystemForAvailablePrinters();

    // Read in the file
    QFile file(".__temp_printers.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream stream(&file);
    printers = extractPrintersFromStream(stream);

    // clean up the temporary file
    std::remove(".__temp_printers.txt");

    return printers;
}

} // end Viewer
