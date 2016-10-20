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

#ifndef GATELISTREQUESTHANDLER_H
#define GATELISTREQUESTHANDLER_H

#include <GateInfo.h>

#include <QObject>
#include <QNetworkAccessManager>

#include <vector>
#include <memory>

//Forward declarations
class QNetworkReply;


namespace Viewer
{

/*!
 * \brief The GateListRequestHandler class
 *
 * Provides the logic for requesting the list of known gates and also
 * for handling the response of the server.
 *
 */
class GateListRequestHandler : public QObject
{
    Q_OBJECT
public:
    explicit GateListRequestHandler(QObject *parent = 0);
    
    /////////////////////////////////////////////////////////////////////////
public slots:
    /*!
     * \brief Perform the request operation
     */
    void get();

    /*!
     * \brief Handle the response of the server
     *
     * \param reply     the server's response
     *
     *  This is connected with the finished() signal of the network access manager
     *  during construction.
     *
     * This results in the emission of the parseCompleted signal. An error
     * simply causes the parseCompleted signal to contain an empty vector.
     */
    void finishedSlot(QNetworkReply* reply);

    /////////////////////////////////////////////////////////////////////////
signals:
     void parseCompleted(std::vector<SpJs::GateInfo*> gateList);

     /////////////////////////////////////////////////////////////////////////
private:
    /*!
     * \brief Transform the list of unique_ptr to bare pointers
     *
     * The signals/slot mechanism does't support unique_ptr so much because I cannot
     * copy a unique_ptr. For that reason, we need to release all of the unique_ptrs
     * and then move the pointer it used to hold into a new vector.
     */
    std::vector<SpJs::GateInfo*> 
      deUniquifyVectorContents(std::vector<std::unique_ptr<SpJs::GateInfo>>& vect);

private:
    std::unique_ptr<QNetworkAccessManager> m_pNAM;
};

} // end of namespace

#endif // GATELISTREQUESTHANDLER_H
