/*
 *
 *    utauVoiceDataBase.h
 *                        (c) HAL 2009-
 *
 * This files is a part of v.Connect.
 * utauVoiceDataBase is a container class of contains UTAU oto.ini
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __utauVoiceDataBase_h__
#define __utauVoiceDataBase_h__

#include "utauParameters.h"
#include "utauFreq.h"
#include "../stand.h"

class utauVoiceDataBase
{
public: // public method

    ~utauVoiceDataBase();

    /// <summary>
    /// oto.ini�̃t�@�C�����Ƃ��̃e�L�X�g�E�G���R�[�f�B���O���w�肵�āCoto.ini��ǂݍ��݂܂��D
    /// </summary>
    /// <param name="path_oto_ini">oto.ini�t�@�C���̃p�X�D</param>
    /// <param name="codepage">oto.ini�t�@�C���̃e�L�X�g�E�G���R�[�f�B���O�D</param>
    int read( string_t fileName, const char *codepage );

    int getUtauParameters( utauParameters& parameters, string_t search );

    int getVoicePath( string_t& dst ){
        dst = voicePath; 
        return 1;
    }

    bool empty( void ){
        return settingMap.empty();
    }


public: // public static method
    
    /// <summary>
    /// �Ǎ��ς݂�UTAU�����̌����擾���܂��D
    /// </summary>
    /// <returns>�Ǎ��ς݂�UTAU�����̌��D</returns>
    static int dbSize();

    /// <summary>
    /// �ǂݍ���UTAU�����̃��X�g��j�����܂��D
    /// </summary>
    static void dbClear();

    /// <summary>
    /// UTAU���������X�g�ɒǉ����܂��D
    /// </sumamry>
    /// <param name="db">�����̃C���X�^���X</param>
    static void dbRegist( utauVoiceDataBase *db );

    /// <summary>
    /// �w�肵���C���f�b�N�X��UTAU�������擾���܂��D
    /// </summary>
    /// <param name="index">�������w�肷��C���f�b�N�X</param>
    /// <returns>UTAU�����D</returns>
    static utauVoiceDataBase *dbGet( int index );


protected: // protected field

    string_t voicePath;
    map_t<string_t, utauParameters *> settingMap;
    list<utauParameters *> settingList;


private: // private static field

    /// <summary>
    /// ���s���ɓǂݍ��܂ꂽUTAU�����̃��X�g�ł��D
    /// </summary>
    static vector<utauVoiceDataBase *> mDBs;
};

#endif
