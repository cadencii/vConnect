#ifndef TRANSCRIBERSETTING_H
#define TRANSCRIBERSETTING_H

#include <QVector>

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
    stand::io::UtauLibrary *base;
    QVector<stand::io::UtauLibrary *> optionals;

    int note;
    int brightness;

    unsigned int numThreads;
};

}
}


#endif // TRANSCRIBERSETTING_H
