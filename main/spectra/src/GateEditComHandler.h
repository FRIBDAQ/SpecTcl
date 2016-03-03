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

#ifndef GATEEDITCOMHANDLER_H
#define GATEEDITCOMHANDLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <memory>

class QString;
class QNetworkReply;
class QUrl;

namespace Viewer
{

/*!
 * \brief Interacts with REST server to edit a gate
 *
 * This provides the logic for interacting with the REST server
 * during edit requests. It sends the request and then handles the
 * response. There is an assumption made that this will take a short amount
 * of time, because the logic blocks while the waiting for a response.
 *
 */
class GateEditComHandler : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor
     *
     * \param parent    owner
     */
    explicit GateEditComHandler(QObject *parent = 0);
    
    /*!
     * \brief Send the request, wait for response, process it
     *
     * \param req   the request url
     *
     * This method blocks until the interaction is complete between the
     * server and this.
     */
    void makeRequest(const QUrl& req);

signals:
    void completed();
    void error(int code, const QString& errMsg);

private:
    /*!
     * \brief Dispatches the response based on error code
     *
     * \param reply the reply
     */
    void processReply(QNetworkReply* reply);
    void processSuccess(QNetworkReply* reply);
    void processFailure(QNetworkReply* reply);

private:
    std::unique_ptr<QNetworkAccessManager> m_pNAM;
};

} // end of namespace

#endif // GATEEDITCOMHANDLER_H

