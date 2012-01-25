#ifndef TRANSCRIBERWIDGET_H
#define TRANSCRIBERWIDGET_H

#include <QWidget>

class QTextCodec;

namespace Ui {
class TranscriberWidget;
}

namespace stand
{
namespace gui
{

class TranscriberWidget : public QWidget
{
    Q_OBJECT
public slots:
    void openDirDialog();
    void settingChanged();

signals:
    void changed(QWidget *);

public:
    explicit TranscriberWidget(QWidget *parent = 0, int index = 0ULL);
    ~TranscriberWidget();

    QString dir();
    QString colorName();
    int bri();
    int note();
    QTextCodec *codec();

private:
    Ui::TranscriberWidget *ui;
};

}
}

#endif // TRANSCRIBERWIDGET_H
