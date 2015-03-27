#include "ContentRequestHandler.h"
#include "GlobalSettings.h"
#include "HistogramList.h"
#include "HistogramBundle.h"
#include "LockGuard.h"
#include "Compression.h"
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

namespace cprs = Compress;
using namespace std;

ContentRequestHandler::ContentRequestHandler(QObject *parent) :
    QThread(parent),
    m_nam(),
    m_requests(),
    m_mutex(),
    m_cond(),
    m_quit(false)
{
}

////
//
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
        std::cout << "Not running... need to start" << std::endl;

        start();

        std::cout << "Started" << std::endl;
    } else if (m_requests.size()==1) {
        std::cout << "Already running... must wake" << std::endl;

        m_cond.wakeOne();

        std::cout << "Done waking up the condition" << std::endl;
    } else {
      std::cout << "Processing an earlier request... your request has been queued." << std::endl;
    }
}

////
//
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


    std::cout << "Processing GET " << req.url().toString().toStdString() << std::endl;
    std::unique_ptr<QNetworkReply> reply = doGet(req);
    std::cout << "Received data" << std::endl;

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


void ContentRequestHandler::updateRequest()
{
    QString host = GlobalSettings::getInstance()->value("/server/hostname").toString();
    int port = GlobalSettings::getInstance()->value("/server/port").toInt();

    QString reqStrTemplate("http://%1:%2/spectcl/spectrum/contents?name=");
    reqStrTemplate = reqStrTemplate.arg(host).arg(port);

    auto names = HistogramList::histNames();
    auto iter = names.begin();
    auto end  = names.end();
    while (iter!=end) {

        QString request = (reqStrTemplate+(*iter));
        get(request);

        ++iter;
    }

};

QString ContentRequestHandler::getHistNameFromRequest(const QNetworkRequest &request)
{
    QUrl url = request.url();
    QString name = url.queryItemValue("name");
    return name;
}

std::unique_ptr<QNetworkReply> ContentRequestHandler::doGet(const QNetworkRequest& req)
{
  // GET
  std::unique_ptr<QNetworkReply> reply(m_nam.get(req));

  // Wait for the reply
  QEventLoop eLoop;
  QObject::connect(&m_nam, SIGNAL(finished(QNetworkReply*)),
                   &eLoop, SLOT(quit()));
  eLoop.exec();

  // All done ... return reply
  return std::move(reply);
}


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
        cout << "uncompress..." << flush;
        size_t nBytesDecomp = reply->rawHeader("Uncompressed-Length").toInt();

        json = cprs::uncompress(nBytesDecomp, bytes);
        cout << "done" << endl;
    } else {
        json = bytes;
    }

    cout << "reading json..." << flush;
    bool ok = reader.parse(json.constData(),value);
    cout << "done" << endl;
    if (!ok) {
      throw std::runtime_error ("Failed to parse json");
    }

    // parse the content... this will throw if the json
    // specifies a status other than "ok"
    cout << "parsing json..." << flush;
    auto content = SpJs::JsonParser().parseContentCmd(value);
    cout << "done" << endl;


    // get the name of the hist and update it if it exists.
    auto name = getHistNameFromRequest(reply->request());
    auto histBundle = HistogramList::getHist(name);
    if (histBundle->hist()) {
      LockGuard<HistogramBundle> lock(*histBundle);
      SpJs::HistFiller()(*(histBundle->hist()), content);
    }

    // ALL DONE!
    emit parsingComplete(histBundle);

  } catch (std::exception& exc) {
    QString msg("Failed to update hist because : %1");
    msg = msg.arg(QString(exc.what()));

    // FAIL....
    emit this->error(0, msg);
  }
}

void ContentRequestHandler::completeJob()
{
  QMutexLocker lock(&m_mutex);
  m_requests.pop_front();

  // If we there are no more jobs to process, wait for a new one
  if (m_requests.empty()) {
    m_cond.wait(&m_mutex);
  } 
}

