#ifndef SPECGRAMVIEW_H
#define SPECGRAMVIEW_H

#include <QWidget>
#include <QImage>

#include "../configure.h"

namespace stand
{
namespace math
{
class SpecgramSet;
}
namespace gui
{

class SpecgramView : public QWidget
{
    Q_OBJECT
public:
    enum ScaleType
    {
        Unknown = -1,
        Linear = 0,
        Log
    };

    struct Setting
    {
        stand::math::SpecgramSet *specgram;
        double frequencyCeil;
        double frequencyFloor;
        ScaleType scaleType;
    };

    explicit SpecgramView(stand::math::SpecgramSet *specgram = 0, double frequencyFloor = 0.0, double frequencyCeil = 0.0, ScaleType type = Linear, QWidget *parent = 0);
    explicit SpecgramView(const stand::gui::SpecgramView::Setting &setting, QWidget *parent = 0);
    explicit SpecgramView(QWidget *parent = 0);
    virtual ~SpecgramView();

    virtual void paintEvent(QPaintEvent *e);

    double frequencyByPosition(int y);
    int positionByFrequency(double freq);

signals:

public slots:
    virtual void setSpecgram(stand::math::SpecgramSet *specgram, double frequencyFloor, double frequencyCeil, ScaleType type);
    virtual void setSpecgram(const stand::gui::SpecgramView::Setting &setting);

protected:
    void paint(QPaintEvent *e);
    void createImage(const stand::gui::SpecgramView::Setting &setting);
    QImage _image;

    Setting _setting;
    double _maximumValue;
};

}
}


#endif // SPECGRAMVIEW_H
