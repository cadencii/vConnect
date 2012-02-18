#ifndef AUDIOMIXER_H
#define AUDIOMIXER_H

#include <QObject>
#include <QAudioOutput>
#include <QVector>

#include "StreamDevice.h"
#include "AudioTrack.h"

namespace stand
{
namespace io
{
namespace audio
{

class AudioMixer : public QObject
{
    Q_OBJECT
public:
    explicit AudioMixer(const QAudioFormat &format, QObject *parent = 0);
    ~AudioMixer();

    void addTrack(AudioTrack *t, int volume, int pan);
    void removeTrack(AudioTrack *t);

    void clear();

    void setFormat(const QAudioFormat &format);
    struct Track
    {
        AudioTrack *t;
        int volume;
        int pan;
    };

signals:
    void errorEmpty();


public slots:
    void start();
    void stop();
    void mixTracks(qint64 length);

private:
    void _destroy();
    qint64 _pos;
    QAudioFormat _format;
    QVector<Track> _tracks;
    QAudioOutput *_output;
    StreamDevice *_device;
};

}
}
}

#endif // AUDIOMIXER_H
