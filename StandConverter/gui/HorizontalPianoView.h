#ifndef HORIZONTALPIANOVIEW_H
#define HORIZONTALPIANOVIEW_H

#include <QWidget>

namespace stand
{
namespace gui
{

class HorizontalPianoView : public QWidget
{
    Q_OBJECT
public:
    explicit HorizontalPianoView(QWidget *parent = 0);
    virtual ~HorizontalPianoView();

    virtual void paintEvent(QPaintEvent *e);

protected:
    static const int _MINIMUM_WIDTH;
    static const int _MINIMUM_HEIGHT;
    static const bool _IS_BLACK[12];
signals:

public slots:

};

}
}

#endif // HORIZONTALPIANOVIEW_H
