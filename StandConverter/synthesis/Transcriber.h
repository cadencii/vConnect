#ifndef TRANSCRIBER_H
#define TRANSCRIBER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QVector>

#include "TranscriberSetting.h"

namespace stand
{
namespace synthesis
{

class TranscriberElement;

class Transcriber : public QThread
{
    Q_OBJECT
signals:
    void progressChanged(int val);
    void finish(bool flag);

public slots:
    void cancel();
    void elementFinished(stand::synthesis::TranscriberElement *e);

public:
    explicit Transcriber(const TranscriberSetting &s, QObject *parent = 0);
    ~Transcriber();

    void run();

private:
    TranscriberSetting setting;
    int currentFinished;
    int currentIndex;
    QVector<TranscriberElement *> elements;

    QMutex waitMutex;
    QWaitCondition condition;
};

}
}

#endif // TRANSCRIBER_H
