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
//
#ifndef COMMONRESPONSEHANDLER_H
#define COMMONRESPONSEHANDLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <memory>

class QString;
class QNetworkReply;
class QUrl;

namespace Viewer
{

/*!
 * \brief CommonResponseHandler
 *
 * This handles requests that require no response handling besides checking
 * for success. This is useful for operations that don't return information.
 * For example, a gate edit request uses this.
 *
 */
class CommonResponseHandler : public QObject
{
    Q_OBJECT

public:
    explicit CommonResponseHandler(QObject *parent = 0);
    
    void makeRequest(const QUrl& req);

signals:
    void completed();
    void error(int code, const QString& errMsg);

public slots:
    void processReply(QNetworkReply* reply);

private:
    void processSuccess(QNetworkReply* reply);
    void processFailure(QNetworkReply* reply);

private:
    std::unique_ptr<QNetworkAccessManager> m_pNAM;
};

} // end namespace

#endif // GATEEDITCOMHANDLER_H
