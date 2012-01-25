#include "TranscriberWidget.h"
#include "ui_TranscriberWidget.h"

#include <QStringList>
#include <QFileDialog>
#include <QDir>
#include <QTextCodec>

using namespace stand::gui;

TranscriberWidget::TranscriberWidget(QWidget *parent, int index) :
    QWidget(parent),
    ui(new Ui::TranscriberWidget)
{
    ui->setupUi(this);

    // Set ColorComboBox
    QStringList colorNames;
    colorNames << tr("red") << tr("green") << tr("blue") << tr("cyan") << tr("magenta") << tr("yellow") << tr("white");
    QPalette pallete = ui->ColorSelector->palette();
    pallete.setColor(QPalette::Highlight, Qt::transparent);

    int size = ui->ColorSelector->style()->pixelMetric(QStyle::PM_SmallIconSize);
    QPixmap pixmap(size, size);

    for(unsigned int i = 0; i < colorNames.size(); i++)
    {
        QString name = colorNames.at(i);
        ui->ColorSelector->addItem(name, QColor(i));
        pixmap.fill(QColor(name));
        ui->ColorSelector->setItemData(i, pixmap, Qt::DecorationRole);
    }
    ui->ColorSelector->setCurrentIndex(index % ui->ColorSelector->count());
}

TranscriberWidget::~TranscriberWidget()
{
    delete ui;
}

void TranscriberWidget::settingChanged()
{
    emit changed(this);
}

QString TranscriberWidget::dir()
{
    return ui->DirectoryName->text();
}

int TranscriberWidget::bri()
{
    return ui->BirghtnessSpinBox->value();
}

int TranscriberWidget::note()
{
    return ui->NoteSpinBox->value();
}

void TranscriberWidget::openDirDialog()
{
    QString dirName = QFileDialog::getExistingDirectory(
                this,
                tr("Select input directory."),
                tr("")
                );
    ui->DirectoryName->setText(dirName);
}

QTextCodec *TranscriberWidget::codec()
{
    return QTextCodec::codecForName(ui->EncodeComboBox->currentText().toLocal8Bit().data());
}

QString TranscriberWidget::colorName()
{
    return ui->ColorSelector->currentText();
}
