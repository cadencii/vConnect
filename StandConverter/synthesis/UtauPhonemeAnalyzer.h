#ifndef UTAUPHONEMEANALYZER_H
#define UTAUPHONEMEANALYZER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "../io/UtauLibrary.h"
#include "ConverterSetting.h"

namespace stand
{
namespace io
{
class WaveFile;
}
namespace synthesis
{

class ConverterSetting;
class Converter;

class UtauPhonemeAnalyzer : public QThread
{
    Q_OBJECT
signals:

public slots:

public:
    explicit UtauPhonemeAnalyzer(unsigned int index, const ConverterSetting &s, Converter *c, QMutex *m = NULL);

    void run();
    void setIndex(unsigned int index);
    void converterFinished();
private:
    void _writeDebugString();
    void _analyze();

    stand::io::UtauPhoneme target;
    ConverterSetting setting;
    unsigned int index;

    QString directory;
    QMutex *mutex;
    Converter *converter;
    QWaitCondition condition;

    bool _done;
};

}
}

#endif // UTAUPHONEMEANALYZER_H
