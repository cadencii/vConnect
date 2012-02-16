/*!
 * @file ConverterSetting.h
 * @author HAL
 */
#ifndef CONVERTERSETTING_H
#define CONVERTERSETTING_H

#include <QDir>
#include <QTextCodec>

namespace stand
{
namespace io
{
class UtauLibrary;
}

namespace synthesis
{

/// <summary>
/// UTAU 音源→ v.Connect 音源を変換する設定です．
/// </summary>
class ConverterSetting
{
public:
    stand::io::UtauLibrary *library;
    QDir outDir;
    QTextCodec *outCodec;

    unsigned int cepstrumLength;
    unsigned int vorbisBitrate;
    bool normalizeUtauLibrary;
    bool overwrite;
    float preUtterance;
    float phonemeLength;

    double f0Ceil;
    double f0Floor;

    double framePeriod;

    unsigned int numThreads;
};

}
}

#endif // CONVERTERSETTING_H
