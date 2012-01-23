#include <QtGui/QApplication>
#include "gui/ConverterWindow.h"
#include "gui/TranscriberWindow.h"

#include <QTextCodec>

using namespace stand::gui;

// コンバータ
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // ソースコードが UTF-8 なので内部は UTF-8 で統一．
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForCStrings(codec);
    //QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForTr(codec);

    // 実行
//    ConverterWindow w;
    TranscriberWindow w;
    w.show();
    
    return a.exec();
}
