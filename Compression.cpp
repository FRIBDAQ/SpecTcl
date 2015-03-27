
#include "Compression.h"
#include <QByteArray>
#include <zlib.h>
#include <stdexcept>
#include <iostream>

namespace Compress 
{
  
  QByteArray uncompress(int nUCBytes, const QByteArray& cBuffer) {

    unsigned          allocationUnit = 2<<20; // start at 4 Mbyte
    int               status = Z_BUF_ERROR;
    unsigned long     uclength = allocationUnit;
    QByteArray        ucBuffer;

//    try {

      // resize
      ucBuffer.reserve(nUCBytes);

      while(status == Z_BUF_ERROR) {

        uclength = ucBuffer.capacity();
        auto* pucBuffer = reinterpret_cast<unsigned char*>(ucBuffer.data());
        auto* pcBuffer = reinterpret_cast<const unsigned char*>(cBuffer.data());
        status = ::uncompress(pucBuffer, &uclength, pcBuffer, cBuffer.size());

        if (status == Z_OK) break;

        // In case we need to enlarge increase by the original size every time
        uclength += allocationUnit; 
        ucBuffer.reserve(uclength);
        std::cout << "Had to increase size" << std::endl;
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
//    }
//    catch (const std::exception& msg) {
//      std::cout << msg.what() << std::endl;
//    }

    return ucBuffer;
  };


}
