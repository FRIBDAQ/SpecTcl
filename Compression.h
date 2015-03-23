
#ifndef COMPRESSION_H
#define COMPRESSION_H

class QByteArray;

namespace Compress
{

  extern QByteArray uncompress(const QByteArray& comprData);
//  extern QByteArray compress(const QByteArray& uncomprData);

}

#endif
