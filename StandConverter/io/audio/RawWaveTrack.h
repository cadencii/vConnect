#ifndef RAWWAVETRACK_H
#define RAWWAVETRACK_H

#include "AudioTrack.h"

namespace stand
{
namespace io
{
class WaveFile;
namespace audio
{

class RawWaveTrack : public AudioTrack
{
    Q_OBJECT
public:
    explicit RawWaveTrack(const stand::io::WaveFile *w, const QAudioFormat &format, QObject *parent);
    ~RawWaveTrack();

public slots:
    void generateBuffer(double msPos, double *buffer, qint64 &length);
    void generateBuffer(qint64 samplePos, double *buffer, qint64 &length);

private:
    const stand::io::WaveFile *_wave;
};

}
}
}

#endif // RAWWAVETRACK_H
