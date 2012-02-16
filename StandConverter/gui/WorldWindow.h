#ifndef WORLDWINDOW_H
#define WORLDWINDOW_H

#include <QMainWindow>
#include "../io/SpecgramImage.h"

namespace Ui {
class WorldWindow;
}

namespace stand
{
namespace math
{
class SpecgramSet;
namespace world
{
class WorldSet;
}
}
namespace io
{
class WaveFile;
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

private:
    void _destroy();
    Ui::WorldWindow *ui;
    stand::math::SpecgramSet *_s;
    stand::math::world::WorldSet *_w;
    stand::io::WaveFile *_wave;

    stand::io::SpecgramImage _imagePowerSpec;
    stand::io::SpecgramImage _imageStarSpec;
};

}
}

#endif // WORLDWINDOW_H
