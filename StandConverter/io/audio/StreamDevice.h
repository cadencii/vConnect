#ifndef STREAMDEVICE_H
#define STREAMDEVICE_H

#include <QIODevice>
#include <QAudioFormat>

namespace stand
{
namespace io
{
namespace audio
{

class StreamDevice : public QIODevice
{
    Q_OBJECT
public:
    explicit StreamDevice(const QAudioFormat &format, qint64 length, QObject *parent = 0);
    ~StreamDevice();

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 bytesAvailable();

    const QAudioFormat &format()
    {
        return _format;
    }

    void start();
    void stop();

signals:
    void requireBuffer(qint64);

public slots:

private:
    void _create(const QAudioFormat &format, qint64 length);
    void _destroy();

    qint16 *_buffer;
    qint64 _length;

    qint64 _pos;
    qint64 _bytesAvailable;
    QAudioFormat _format;

    const static int MINIMUM_LENGTH;
};

}
}
}

#endif // STREAMDEVICE_H
