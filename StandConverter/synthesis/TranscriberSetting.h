#ifndef TRANSCRIBERSETTING_H
#define TRANSCRIBERSETTING_H

#include <QVector>
#include "Transcriber.h"

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
    Transcriber::TranscriberItem base;
    QVector<Transcriber::TranscriberItem> optionals;

    unsigned int numThreads;
};

}
}


#endif // TRANSCRIBERSETTING_H
