#ifndef TRANSCRIBERELEMENT_H
#define TRANSCRIBERELEMENT_H

#include <QThread>
#include <QVector>

#include "Transcriber.h"

class QMutex;

namespace stand
{
namespace io
{
class UtauLibrary;
}
namespace synthesis
{

class TranscriberSetting;

class TranscriberElement : public QThread
{
    Q_OBJECT
public slots:
public:
    explicit TranscriberElement(unsigned int index, const TranscriberSetting *s, QMutex *m, Transcriber *t);

    void run();
    void setIndex(int index);
    void finishTranscription();
private:
    unsigned int index;
    const TranscriberSetting *setting;
    QMutex *mutex;
    Transcriber *transcriber;

    QVector<Transcriber::TranscriberItem> items;

    bool isFinished;
};

}
}

#endif // TRANSCRIBERELEMENT_H
