/*!
 * @file Converter.h
 * @author HAL
 */
#ifndef CONVERTER_H
#define CONVERTER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "ConverterSetting.h"

namespace stand
{
namespace synthesis
{

class UtauPhonemeAnalyzer;

class Converter : public QThread
{
    Q_OBJECT
signals:
    void progressChanged(int val);
    void finish(bool flag);

public slots:
    void cancel();

public:
    explicit Converter(const ConverterSetting &s, QWidget *parent = NULL);
    virtual ~Converter();

    void run();

    void recieveRightBlank(int index, float rightBlank);
    void analyzerFinished(stand::synthesis::UtauPhonemeAnalyzer *, bool);

private:
    void _initializeAnalyzers();
    void _writeOtoIni();

    float *rightBlanks;
    UtauPhonemeAnalyzer **analyzers;
    ConverterSetting setting;
    bool isCanceled;
    volatile unsigned int currentPosition;
    volatile unsigned int currentFinished;
    QMutex mutex, waitMutex;
    QWaitCondition condition;
    int ignoreFlag;
};

}
}

#endif // CONVERTER_H
