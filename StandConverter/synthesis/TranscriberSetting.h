#ifndef TRANSCRIBERSETTING_H
#define TRANSCRIBERSETTING_H

#include <QDir>
#include <QVector>
#include "Transcriber.h"

class QTextCodec;

namespace stand
{
namespace io
{
class UtauLibrary;
}
namespace synthesis
{

class TranscriberSetting
{
public:
    QString libraryName;
    QString libraryIconPath;
    QString libraryAuthor;
    QString libraryWeb;
    QVector<Transcriber::TranscriberItem> libraries;

    unsigned int numThreads;
    QDir root;
    QTextCodec *codec;
};

}
}


#endif // TRANSCRIBERSETTING_H
