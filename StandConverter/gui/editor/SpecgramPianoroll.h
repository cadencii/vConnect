#ifndef SPECGRAMPIANOROLL_H
#define SPECGRAMPIANOROLL_H

#include "../SpecgramView.h"

#include <QVector>

namespace stand
{
namespace utility
{
class F0Contour;
}
namespace gui
{
namespace editor
{

class SpecgramPianoroll : public SpecgramView
{
    Q_OBJECT
public:
    explicit SpecgramPianoroll(const Setting &setting, int width, int noteHeight, QWidget *parent = 0);
    ~SpecgramPianoroll();

    void paintEvent(QPaintEvent *e);

    void addF0Contour(stand::utility::F0Contour *f0, bool autodelete = false);
    void removeF0Contour(stand::utility::F0Contour *f0);
    int indexOf(stand::utility::F0Contour *f0);
    stand::utility::F0Contour *f0At(int index);

public slots:
    void setSpecgram(const stand::gui::SpecgramView::Setting &setting);

private:
    void _paintPitch(QPainter *painter, QPaintEvent *e);
    int _noteHeight;
    static QColor Colors[];

    struct PitchItem
    {
        stand::utility::F0Contour *p;
        bool autodelete;
    };

    QVector<PitchItem> _pitches;
};

}
}
}

#endif // SPECGRAMPIANOROLL_H
