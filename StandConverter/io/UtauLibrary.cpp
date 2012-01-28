#include "UtauLibrary.h"

#define DEBUG

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QStringList>

using namespace stand::io;

UtauLibrary::UtauLibrary(const QString &filename, QTextCodec *codec, int maxDepth)
{
    _directory = QDir("");
    if(!filename.isEmpty())
    {
        readRecursive(filename, codec, maxDepth);
    }
}

bool UtauLibrary::readFromOtoIni(const QString &filename, QTextCodec *codec)
{
    QFileInfo info(filename);
    _directory = info.absoluteDir().absolutePath();
    clear();
    return _readFromOtoIni(filename, codec);
}

bool UtauLibrary::_readFromOtoIni(const QString &filename, QTextCodec *codec)
{
    QFile file(filename);
    qDebug("UtauLibrary::readFromOtoIni(%s, %s)", filename.toLocal8Bit().data(), (codec)?(codec->name().data()):"\"\"");

    // ファイルが存在しない．
    if(file.exists() == false)
    {
        qDebug(" File Not Found :%s", filename.toLocal8Bit().data());
        return false;
    }

    // 開けない．
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug(" File Open Error :%s", filename.toLocal8Bit().data());
        return false;
    }

    // If there is no codec, set Locale as default.
    if(!codec)
    {
        codec = QTextCodec::codecForLocale();
    }

    QFileInfo fileInfo(filename);
    QString relative = _directory.relativeFilePath(fileInfo.absoluteDir().absolutePath());
    if(relative == QDir::separator())
    {
        relative = "";
    }
    relative = QDir::toNativeSeparators(relative);
    QTextStream stream(&file);
    stream.setCodec(codec);

    int c = 0;
    while(!stream.atEnd())
    {
        QString line;
        line = stream.readLine();
        c++;
        if(!_readOneLine(line, relative))
        {
            qDebug(" Invalid args in l.%d\t: %s", c, line.toLocal8Bit().data());
        }
    }

    bool ret = (!this->settingList.empty());
    qDebug(" Finish reading %d lines in %s.", c, ret?"success":"faiure");

    file.close();
    // 一つでも要素が読み込めれば読めたことにする．
    return ret;
}

bool UtauLibrary::readRecursive(const QString &filename, QTextCodec *codec, int maxDepth)
{
    _directory = QDir(filename);
    clear();
    return _readRecursive(filename, codec, maxDepth);
}

bool UtauLibrary::_readRecursive(const QString &filename, QTextCodec *codec, int maxDepth)
{
    qDebug("UtauLibrary::readRecursive(%s, %s, %d)", filename.toLocal8Bit().data(), (codec)?(codec->name().data()):"\"\"", maxDepth);
    bool ret = false;
    QDir dir(filename);
    QFileInfoList dirList = dir.entryInfoList(QDir::AllDirs);
    QString otoIniPath = dir.absolutePath() + QDir::separator() + "oto.ini";

    ret |= _readFromOtoIni(otoIniPath, codec);

    if(!maxDepth)
    {
        return ret;
    }
    for(int i = 0; i < dirList.size(); i++)
    {
        if( dirList.at(i).fileName() == QString("..")
            || dirList.at(i).fileName() == QString(".")
            )
        {
            continue;
        }
        ret |= _readRecursive(dir.absolutePath() + QDir::separator() + dirList.at(i).fileName(), codec, maxDepth - 1);
    }
    return ret;
}

bool UtauLibrary::_readOneLine(QString &line, QString &dir)
{
    // 読むものがないので帰ります．
    if(line.isEmpty())
    {
        return false;
    }

    QStringList list = line.split(",");
    QStringList names;
    // Invalid args.
    if(list.size() != 6)
    {
        return false;
    }
    // Parse args.
    UtauPhoneme setting;
    names = list.at(0).split("=");
    if(names.size() == 1)
    {
        setting.filename = names.at(0);
        names = setting.filename.split(".");
        if(names.size() < 2)
        {
            return false;
        }
        setting.pronounce = names.at(0);
    }
    else if(names.size() == 2)
    {
        setting.filename = names.at(0);
        setting.pronounce = names.at(1);
    }
    else
    {
        return false;
    }
    setting.filename = dir + setting.filename;
    setting.leftBlank = list.at(1).toFloat();
    setting.fixedLength = list.at(2).toFloat();
    setting.rightBlank = list.at(3).toFloat();
    setting.preUtterance = list.at(4).toFloat();
    setting.voiceOverlap = list.at(5).toFloat();

    push_back(setting);
    return true;
}

const UtauPhoneme *UtauLibrary::at(unsigned int i)
{
    const UtauPhoneme *ret = NULL;
    if(0 <= i && i < settingList.size())
    {
        ret = &(settingList.at(i));
    }
    return ret;
}

const UtauPhoneme *UtauLibrary::find(const QString &pronounce)
{
    QHash<QString, int>::iterator it;
    it = settingMap.find(pronounce);
    if(it != settingMap.end())
    {
        return at(it.value());
    }
    return NULL;
}

bool UtauLibrary::write(const QString &filename, QTextCodec *codec)
{
    QFile file(filename);
    // 開けない．
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug("UtauLibrary::write; File Open Error :%s", filename);
        return false;
    }

    // If there is no codec, set Locale as default.
    if(!codec)
    {
        codec = QTextCodec::codecForLocale();
    }

    QTextStream data;
    data.setDevice(&file);
    data.setCodec(codec);
    for(int i = 0; i < settingList.size(); i++)
    {
        const UtauPhoneme *p = &(settingList.at(i));
        data << p->filename
             << "=" << p->pronounce
             << "," << p->leftBlank
             << "," << p->fixedLength
             << "," << p->rightBlank
             << "," << p->preUtterance
             << "," << p->voiceOverlap
             << "\n";
    }
    file.close();
    return true;
}
