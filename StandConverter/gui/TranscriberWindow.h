#ifndef TRANSCRIBERWINDOW_H
#define TRANSCRIBERWINDOW_H

#include <QMainWindow>

namespace Ui {
class TranscriberWindow;
}

namespace stand
{
namespace gui
{

class TranscriberWindow : public QMainWindow
{
    Q_OBJECT
public slots:
    void addTab();
    void removeTab();
public:
    explicit TranscriberWindow(QWidget *parent = 0);
    ~TranscriberWindow();

    void closeEvent(QCloseEvent *e);

private:
    Ui::TranscriberWindow *ui;
};

}
}

#endif // TRANSCRIBERWINDOW_H
