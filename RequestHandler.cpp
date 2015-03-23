#include "RequestHandler.h"
#include "GlobalSettings.h"
#include "HistogramList.h"
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


RequestHandler::RequestHandler(QObject *parent) :
    QThread(parent),
    m_url(),
    m_mutex(),
    m_cond(),
    m_quit(false)
{}

RequestHandler::~RequestHandler()
{
    m_mutex.lock();
    m_quit = true;
    m_cond.wakeOne();
    m_mutex.unlock();
    wait();

}

void RequestHandler::get(const QUrl& url)
{
    QMutexLocker lock(&m_mutex);
    m_url = url;
    if (!isRunning()) {
        std::cout << "Not running... need to start" << std::endl;
        start();
        std::cout << "Started" << std::endl;
    } else {
        std::cout << "Already running... must wake" << std::endl;
        m_cond.wakeOne();
        std::cout << "Done waking up the condition" << std::endl;
    }
}

void RequestHandler::run()
{
    m_mutex.lock();
    QUrl url = m_url;
    m_mutex.unlock();

    QNetworkAccessManager nam;

    while (!m_quit) {

        std::unique_ptr<QNetworkReply> reply(nam.get(QNetworkRequest(url)));

        std::cout << reply->request().url().toString().toStdString() << std::endl;
        std::cout << "Fetching data" << std::endl;

        QEventLoop eLoop;
        QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)),
                         &eLoop, SLOT(quit()));
        eLoop.exec();

        std::cout << "Received data" << std::endl;


        auto error = reply->error();
        if (error == QNetworkReply::NoError) {
            QByteArray bytes = reply->readAll();

            try {
                // Read the response in the json
                Json::Reader reader;
                Json::Value value;

                QByteArray json = decompress(bytes);

                std::ofstream ofile("json_decomp.out");
                ofile << json.constData() << std::endl;
                ofile.close();

                bool ok = reader.parse(json.constData(),value);
                if (!ok) {
                    throw std::runtime_error ("Failed to parse json");
                }

                // parse the content... this will throw if the json
                // specifies a status other than "ok"
                auto content = SpJs::JsonParser().parseContentCmd(value);

                // get the name of the hist and update it if it exists.
                auto name = getHistNameFromRequest(reply->request());
                const auto& guardedHist = HistogramList::getHist(name);
                if (guardedHist.hist()) {
                    LockGuard<GuardedHist> lock(guardedHist);
                    SpJs::HistFiller()(*(guardedHist.hist()), content);
                }

                emit parsingComplete(&guardedHist);

                std::cout << "Done parsing... now waiting" << std::endl;


            } catch (std::exception& exc) {
                QString msg("Failed to update hist because : %1");
                msg = msg.arg(QString(exc.what()));
                std::cout << msg.toStdString() << std::endl;
                emit this->error(0, msg);
            }
        } else {
            std::cout << "error " << error << ":" << reply->errorString().toStdString() << std::endl;
            emit this->error(error, reply->errorString());
        }

        // set up for next iteration
        m_mutex.lock();
        m_cond.wait(&m_mutex);
        url = m_url;
        m_mutex.unlock();

    } // end of while
}


void RequestHandler::updateRequest()
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

QString RequestHandler::getHistNameFromRequest(const QNetworkRequest &request)
{
    QUrl url = request.url();
    QString name = url.queryItemValue("name");
    return name;
}


QByteArray RequestHandler::decompress(const QByteArray& comprStr)
{
  QByteArray pUncompressed;
  try {
    unsigned length = 4<<20; // start at 4 Mbyte
    int status = Z_BUF_ERROR;
    unsigned uclength = 4*length;
    const QByteArray& pCompressed = comprStr;
    pUncompressed.reserve(uclength);
    while(status == Z_BUF_ERROR) {

      status = unCompress(pUncompressed, pCompressed);
      if (status == Z_OK) break;

      uclength += length; // In case we need to enlarge increase by 4 Mbyte each time
      pUncompressed.reserve(uclength);
    }

    switch (status) {
      case Z_OK:
        break;
      case Z_DATA_ERROR:
        throw std::runtime_error("Input was not compressed with 'deflate'");
      case Z_MEM_ERROR:
        throw std::runtime_error("Zlib internal memory allocation failed");
      default:
        throw std::runtime_error("Unanticipated error from zlib uncompress function");
    }
  }
  catch (const std::exception& msg) {
    std::cout << msg.what() << std::endl;
  }

  return pUncompressed;

}

int RequestHandler::unCompress(QByteArray& ucBuffer, const QByteArray& cBuffer)
{
  unsigned long int uclength = ucBuffer.capacity();
  return uncompress(reinterpret_cast<unsigned char*>(ucBuffer.data()), &uclength, 
                    reinterpret_cast<const unsigned char*>(cBuffer.data()), cBuffer.size());
}
