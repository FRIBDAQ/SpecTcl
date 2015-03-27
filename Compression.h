
#ifndef COMPRESSION_H
#define COMPRESSION_H

class QByteArray;

namespace Compress
{

  extern QByteArray uncompress(int nBytesUC, const QByteArray& comprData);
//  extern QByteArray compress(const QByteArray& uncomprData);

}

#endif
