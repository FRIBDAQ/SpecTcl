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

#include "ContentRequestHandler.h"
#include "GlobalSettings.h"
#include "HistogramList.h"
#include "HistogramBundle.h"
#include "Compression.h"
#include "ConnectionTester.h"

#include <json/json.h>
#include <HistFiller.h>
#include <JsonParser.h>

#include <QtNetwork>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QMutexLocker>
#include <QEventLoop>
#include <QObject>
#include <QHostInfo>

#include <memory>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <mutex>

#include <zlib.h>
#include <cstdlib>

using namespace std;

namespace Viewer
{

namespace cprs = Compression;

//
//
ContentRequestHandler::ContentRequestHandler(HistogramList* pHistList, QObject *parent) :
    QThread(parent),
    m_pHistList(pHistList),
    m_nam(),
    m_requests(),
    m_mutex(),
    m_cond(),
    m_quit(false)
{
}

//
//
ContentRequestHandler::~ContentRequestHandler()
{

    m_mutex.lock();

    // Schedule the thread to stop
    m_quit = true;

    // wake the thread
    m_cond.wakeOne();
    m_mutex.unlock();

    // this should complete the processing
    wait();
}

/*!
 * \brief Queue a request for content
 *
 * \param url  url for the request
 *
 * This is the entry point for requests. It is typically going to be called
 * from a different thread than the thread that is running the logic of the
 * run method. This method can wake the thread that is waiting.
 */
void ContentRequestHandler::get(const QUrl& url)
{
    QMutexLocker lock(&m_mutex);
    m_requests.push_back(QNetworkRequest(url));

    if (!isRunning()) {

        start();

    } else if (m_requests.size()==1) {

        m_cond.wakeOne();

    }
}

/*!
 * \brief The child thread logic
 *
 * This is just a while loop that will process jobs sequentially. Once the
 * queue is empty, it will wait on a condition variable until a new request
 * arrives.
 */
void ContentRequestHandler::run()
{
  QNetworkRequest req;

  while (!m_quit) {

    // initiate new job
    m_mutex.lock();
    if (!m_requests.empty()) {
      req = m_requests.front();
    } else {
      break;
    }
    m_mutex.unlock();


    std::unique_ptr<QNetworkReply> reply = doGet(req);

    auto error = reply->error();
    if (error == QNetworkReply::NoError) {
      // Successful communication

      processReply(reply);

    } else {
      // An error occurred...

      emit this->error(error, reply->errorString());
    }

    // move to the next job... the thread may block if there are no more jobs.
    completeJob();

  } // end of while
}

/*!
 * \brief getHistNameFromRequest
 * \param request the url of the request
 * \return the name of the histogram
 */
QString ContentRequestHandler::getHistNameFromRequest(const QNetworkRequest &request)
{
    QUrl url = request.url();
    QString name = url.queryItemValue("name");
    return name;
}

/*!
 * \brief Send the REST request to the server.
 *
 * \param req   network request
 * \return the network reply object
 *
 * Because this is being run in a separate thread than the GUI, there is no reason to
 * handle it asynchronously. This method will block until the full repsonse is received.
 */
std::unique_ptr<QNetworkReply> ContentRequestHandler::doGet(const QNetworkRequest& req)
{
  ConnectionTester::Result connectionStatus;
  do {
      QString host = GlobalSettings::getInstance()->value("/server/hostname").toString();
      QString port = GlobalSettings::getInstance()->value("/server/port").toString();

      QHostInfo info = QHostInfo::fromName(host);
      if (info.error() != QHostInfo::NoError) {
          qDebug() << "Lookup failed:" << info.errorString();
      } else {
          for(auto& address : info.addresses())
              std::cout << "Found address:" << address.toString().toStdString() << std::endl;
      }

      QString server("http://%1:%2/spectcl/spectrum/list");
      server = server.arg(host, port);
      connectionStatus = ConnectionTester::acceptingConnections(server);
      sleep(1);
    } while (connectionStatus != ConnectionTester::SUCCESS);

  // GET
  std::unique_ptr<QNetworkReply> reply(m_nam.get(req));


  // Wait for the reply
  QEventLoop eLoop;
  QObject::connect(&m_nam, SIGNAL(finished(QNetworkReply*)),
                   &eLoop, SLOT(quit()));
  //QObject::connect(&timer, SIGNAL(timeout()), reply.get(), SLOT(abort()));
  eLoop.exec();

  // All done ... return reply
  return std::move(reply);
}

/*!
 * \brief Handles a completed reply
 *
 * \param reply  the response from the server
 *
 * This is where the response is processed.
 */
void ContentRequestHandler::processReply(const std::unique_ptr<QNetworkReply>& reply)
{
  QByteArray bytes = reply->readAll();

  auto hdr = reply->header(QNetworkRequest::ContentTypeHeader);

  try {
    // Read the response in the json
    Json::Reader reader;
    Json::Value value;

    QByteArray json;
    QString encoding = reply->rawHeader("Content-Encoding");
    if (encoding == "deflate") {

        // default uncompressed size to try
        size_t nBytesDecomp = 2<<20;

        // see if we can get a better number to use for our uncompressed size
        if (reply->hasRawHeader("Uncompressed-Length")) {
          // this is when we were told by spectcl
          nBytesDecomp = reply->rawHeader("Uncompressed-Length").toInt();
        } else if (reply->hasRawHeader("Content-Length")) {
            // we can guess based on the compressed size that maybe there was
            // a 7x compression. that seems about right from my experience
            nBytesDecomp = reply->rawHeader("Content-Length").toInt();
            nBytesDecomp *= 7;
        }

        json = cprs::uncompress(nBytesDecomp, bytes);
    } else {
        json = bytes;
    }

    bool ok = reader.parse(json.constData(),value);
    if (!ok) {
      throw std::runtime_error ("Failed to parse json");
    }

    // parse the content... this will throw if the json
    // specifies a status other than "ok"
    auto content = SpJs::JsonParser().parseContentCmd(value);


    // get the name of the hist and update it if it exists.
    auto name = getHistNameFromRequest(reply->request());

    HistogramBundle* pHistBundle = nullptr;
    {
      QMutexLocker listLock(m_pHistList->getMutex());
      pHistBundle = m_pHistList->getHist(name);

        std::lock_guard<HistogramBundle> lock(*pHistBundle);
        SpJs::HistFiller setContent;
        setContent(pHistBundle->getHist(), content.getValues());
        std::map<TVirtualPad*, TH1*> clones = pHistBundle->getClones();
        for (auto& cloneInfo : clones) {
            std::cout << "Updating hist content on " << (void*)(cloneInfo.second) << std::endl;
            setContent(*(cloneInfo.second), content.getValues());
        }

    } // scoping to make sure that the list lock is released
      // b4 passing control to some unknown process.

    std::cout << "done updating content" << std::endl;

    // ALL DONE!
    emit parsingComplete(pHistBundle);

  } catch (std::exception& exc) {
    QString msg("Failed to update histogram content because : %1");
    msg = msg.arg(QString(exc.what()));

    // FAIL....
    emit this->error(0, msg);
  }
}

/*!
 * \brief completeJob
 *
 * This clears the job from the queue. If the queue is empty after this,
 * then thread waits on the condition variable.
 */
void ContentRequestHandler::completeJob()
{
  QMutexLocker lock(&m_mutex);
  m_requests.pop_front();

  // If we there are no more jobs to process, wait for a new one
  if (m_requests.empty()) {
    m_cond.wait(&m_mutex);
  } 
}

}
