#ifndef UTAULIBRARY_H
#define UTAULIBRARY_H

#include <QString>
#include <QHash>
#include <QVector>
#include <QDir>

namespace stand
{
namespace io
{

/// <summary> UTAU 原音設定保持用構造体 </summary>
struct UtauPhoneme
{
    QString filename;
    QString pronounce;

    float leftBlank;
    float fixedLength;
    float rightBlank;
    float preUtterance;
    float voiceOverlap;
};

/// <summary> UTAU 音源読み書きクラス </summary>
class UtauLibrary
{
public:
    explicit UtauLibrary(const QString &filename = QString(), QTextCodec *codec = NULL, int maxDepth = 1);

    /// <summary> 指定された oto.ini からUTAU 音源を読み込みます． </summary>
    /// <param name = "filename"> 読み込む音源の oto.ini パス． </param>
    /// <param name = "codec"> oto.ini の文字コード．デフォルトは Shift-JIS です． </param>
    /// <returns> true: 成功， false: 失敗 </returns>
    bool readFromOtoIni(const QString &filename, QTextCodec *codec = NULL);

    /// <summary> UTAU音源を再帰的に読み込みます．全探索してしまわないように深さで枝を制限できます． </summary>
    /// <param name = "filename"> 読み込む音源の oto.ini パス． </param>
    /// <param name = "codec"> oto.ini の文字コード．デフォルトは Shift-JIS です． </param>
    /// <param name = "maxDepth"> 再起読み込みの制限値． </param>
    /// <returns> true: 成功， false: 失敗 </returns>
    bool readRecursive(const QString &filename, QTextCodec *codec = NULL, int maxDepth = 1);

    /// <summary> UTAU 音源を書き込みます． </summary>
    /// <param name = "filename"> 書き込む音源の oto.ini パス． </param>
    /// <param name = "codec"> oto.ini の文字コード．デフォルトは Shift-JIS です． </param>
    /// <returns> true: 成功， false: 失敗 </returns>
    bool write(const QString &filename, QTextCodec *codec = NULL);

    /// <summary> 指定番の原音設定を返します． </summary>
    /// <param name = "i"> 得たい原音設定のインデックスです． </param>
    /// <returns> NULL: 失敗，それ以外: 成功． </returns>
    const UtauPhoneme *at(unsigned int i);

    /// <summary> UTAU 音源の原音設定の数を返します． </summary>
    unsigned int size()
    {
        return settingList.size();
    }

    /// <summary> UtauLibrary クラスを初期化します． </summary>
    void clear()
    {
        settingList.clear();
        settingMap.clear();
    }

    QDir directory()
    {
        return _directory;
    }

    /// <summary> 原音設定の一要素分を追加します． </summary>
    /// <param name = s> 追加したい原音設定 </parm>
    void push_back(const UtauPhoneme &s)
    {
        UtauPhoneme u = s;
        settingList.push_back(u);
        settingMap.insert(u.pronounce, &(settingList.back()));
    }

    /// <summary> 指定発音の原音設定を返します． </summary>
    /// <param name = "i"> 得たい原音設定の発音． </param>
    /// <returns> NULL: 失敗，それ以外: 成功． </returns>
    const UtauPhoneme *find(const QString &pronounce);

private:
    bool _readFromOtoIni(const QString &filename, QTextCodec *codec = NULL);
    bool _readOneLine(QString &line, QString &dir);
    bool _readRecursive(const QString &filename, QTextCodec *codec = NULL, int maxDepth = 1);

    QVector<UtauPhoneme> settingList;
    QHash<QString, UtauPhoneme *> settingMap;

    QDir _directory;
};

}
}

#endif // UTAULIBRARY_H
