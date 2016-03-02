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
#include "LockGuard.h"
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

#include <memory>
#include <iostream>
#include <stdexcept>
#include <fstream>

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

////
//
//
//
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

//
//
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

//
//
QString ContentRequestHandler::getHistNameFromRequest(const QNetworkRequest &request)
{
    QUrl url = request.url();
    QString name = url.queryItemValue("name");
    return name;
}

//
//
std::unique_ptr<QNetworkReply> ContentRequestHandler::doGet(const QNetworkRequest& req)
{
  ConnectionTester::Result connectionStatus;
  do {
      QString host = GlobalSettings::getInstance()->value("/server/hostname").toString();
      QString port = GlobalSettings::getInstance()->value("/server/port").toString();

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


//
//
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

      if (pHistBundle->hist()) {
        LockGuard<HistogramBundle> lock(pHistBundle);
        SpJs::HistFiller()(*(pHistBundle->hist()), content.getValues());
      }
    } // scoping to make sure that the list lock is released
      // b4 passing control to some unknown process.

    // ALL DONE!
    emit parsingComplete(pHistBundle);

  } catch (std::exception& exc) {
    QString msg("Failed to update hist because : %1");
    msg = msg.arg(QString(exc.what()));

    // FAIL....
    emit this->error(0, msg);
  }
}

//
//
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
