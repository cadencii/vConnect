#ifndef WORLDWINDOW_H
#define WORLDWINDOW_H

#include <QMainWindow>
#include <QAudioFormat>
#include "../io/SpecgramImage.h"

namespace Ui {
class WorldWindow;
}

namespace stand
{
namespace math
{
class SpecgramSet;
class LPCSet;
namespace world
{
class WorldSet;
}
}
namespace io
{
class WaveFile;
namespace audio
{
class AudioMixer;
}
}
namespace gui
{

class WorldWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit WorldWindow(QWidget *parent = 0);
    ~WorldWindow();

public slots:
    void beginAnalyze();
    void searchDirectory();
    void changeSpectrumType(bool b);
    void playSound();

private:
    void _destroy();
    Ui::WorldWindow *ui;
    stand::math::SpecgramSet *_s;
    stand::math::LPCSet *_lpc;
    stand::math::world::WorldSet *_w;
    stand::io::WaveFile *_wave;

    stand::io::SpecgramImage _imagePowerSpec;
    stand::io::SpecgramImage _imageStarSpec;
    stand::io::SpecgramImage _imageLPCSpec;

    QAudioFormat _f;
    stand::io::audio::AudioMixer *_mixer;
};

}
}

#endif // WORLDWINDOW_H
