#ifndef VERTICALPIANOVIEW_H
#define VERTICALPIANOVIEW_H

#include "HorizontalPianoView.h"

namespace stand
{
namespace gui
{

class VerticalPianoView : public HorizontalPianoView
{
    Q_OBJECT
public:
    explicit VerticalPianoView(QWidget *parent = 0);
    ~VerticalPianoView();

    void paintEvent(QPaintEvent *e);
};

}
}

#endif // VERTICALPIANOVIEW_H
