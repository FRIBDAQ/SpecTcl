//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins 
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

#ifndef LISTREQUESTHANDLER_H
#define LISTREQUESTHANDLER_H

#include "QHistInfo.h"

#include <QObject>
#include <QList>
#include <QTimer>
#include <vector>

// forward declarations
class QNetworkReply;
class QNetworkAccessManager;

namespace Viewer
{

// forward declarations
class HistogramView;

/*!
 * \brief The ListRequestHandler class
 *
 * This encapsulates the logic to deal with histogram list requests to the REST
 * server. It sets up the request and then also handles response from the REST
 * server. The requenst handler has a timer object that is used to implement a
 * timeout on the request. The server must begin actively responding within a
 * certain amount of time since the request is made.
 */
class ListRequestHandler : public QObject
{
    Q_OBJECT
public:
    explicit ListRequestHandler(QObject *parent = 0);

    /*!
     * \brief Send the request to the REST server
     */
    void get();

public slots:

    /*!
     * \brief Handle response of the server (dispatch if necessary)
     * \param reply the server response
     */
    void finishedSlot(QNetworkReply* reply);

    /*!
     * \brief Download progress callback to monitor if we have timed out
     */
    void onDownloadProgress(qint64,qint64);

    /*!
     * \brief Logic to execute when the request has timed out
     */
    void onTimeout();

signals:
    /*!
     * \brief Transmit the list of histograms to other parts of the program
     *
     * \param nameList  the parsed list of histograms
     */
    void parseCompleted(std::vector<SpJs::HistInfo> nameList);

private:
    QNetworkReply*         m_pReply;
    QNetworkAccessManager* m_pNAM;
    QTimer*                m_pTimer;
};

} // end of namespace 

#endif // LISTREQUESTHANDLER_H
