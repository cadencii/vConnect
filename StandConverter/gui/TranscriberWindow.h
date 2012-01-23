#ifndef TRANSCRIBERWINDOW_H
#define TRANSCRIBERWINDOW_H

#include <QMainWindow>

namespace Ui {
class TranscriberWindow;
}

class TranscriberWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit TranscriberWindow(QWidget *parent = 0);
    ~TranscriberWindow();
    
private:
    Ui::TranscriberWindow *ui;
};

#endif // TRANSCRIBERWINDOW_H
