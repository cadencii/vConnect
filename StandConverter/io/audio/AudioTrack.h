#ifndef AUDIOTRACK_H
#define AUDIOTRACK_H

#include "StreamDevice.h"

namespace stand
{
namespace io
{
namespace audio
{

class AudioTrack : public QObject
{
    Q_OBJECT
public:
    explicit AudioTrack(const QAudioFormat &format, QObject *parent = 0);
    virtual ~AudioTrack();

public slots:
    virtual void generateBuffer(double msPos, double *buffer, qint64 &length) = 0;
    virtual void generateBuffer(qint64 samplePos, double *buffer, qint64 &length) = 0;

    void setFormat(const QAudioFormat &format);

protected:
    QAudioFormat format;
};

}
}
}

#endif // AUDIOTRACK_H
