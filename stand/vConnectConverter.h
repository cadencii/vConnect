#ifndef __vConnectConverter_h__
#define __vConnectConverter_h__

#include <math.h>
#include "TextReader.h"
#include "TextWriter.h"
#include "waveFileEx/waveFileEx.h"
#include "stand.h"
#include "vConnectPhoneme.h"
#include "StringUtil.h"

using namespace vconnect;

class vConnectConverter
{
public:
    vConnectConverter()
    {
    }

    ~vConnectConverter()
    {
    }

    /**
     * UTAU 音源形式を STAND 音源形式へ変換します． Unicode に対応していない簡易版です．
     * @param otoIni 分析する音源の oto.ini へのパス．
     * @param dstDir 出力先ディレクトリ名．
     * @returns 分析に成功した場合true，それ以外はfalseを返します．
     */
    bool convert( const char *otoIni, const char *dstDir )
    {
        string srcDir = otoIni;
        string dstDir_s = dstDir;
        srcDir = srcDir.substr( 0, srcDir.rfind( PATH_SEPARATOR ) );
        int count = 0;

        TextReader reader( otoIni, "Shift_JIS" );
        TextWriter writer( (dstDir_s + "oto.ini").c_str(), "Shift_JIS", "\x0D\x0A" );
        while( false == reader.isEOF() ){
            string buffer = reader.readLine();
            if( buffer.length() == 0 ){
                continue;
            }
            count++;

            string line = this->processRecord( buffer, count, srcDir, dstDir_s );

            if( line.length() > 0 ){
                writer.writeLine( line );
            }
        }

        writer.close();
        reader.close();

        return true;
    }

private:
    /**
     * 変換元の oto.ini ファイルの１行分のデータを処理する
     * @param record 1 行分のデータ
     * @param count oto.ini の何行目のデータかを表す
     * @param sourceDirectory 変換元音源のディレクトリ
     * @param destinationDirectory 変換後の音源の保存ディレクトリ
     * @return 変換後の oto.ini ファイルに書きこむ行データ
     */
    string processRecord( std::string record, int count, string sourceDirectory, string destinationDirectory )
    {
        list<string> splitted = StringUtil::explode( "=", record, 2 );
        list<string>::iterator i = splitted.begin();
        string fileName = *i++;
        string parametersString = *i++;

        list<string> parameters = StringUtil::explode( ",", parametersString );
        list<string>::iterator j = parameters.begin();
        string alias = *j++;
        float leftBlank = atof( (*j++).c_str() );
        float fixedLength = atof( (*j++).c_str() );
        float rightBlank = atof( (*j++).c_str() );
        float preUtterance = atof( (*j++).c_str() );
        float voiceOverlap = atof( (*j++).c_str() );

        char buf[16];
        sprintf( buf, "%d.vvd", count );
        string vvdName = buf;
        string line = vvdName + "=" + parametersString;

        cerr << "====" << endl;
        cerr << "Begin analysis : " << alias << " @ " << fileName << endl;

        waveFileEx waveFile;
        if( waveFile.readWaveFile( sourceDirectory + PATH_SEPARATOR + fileName ) != 1 ){
            cout << "error ; can't open the file, " << (sourceDirectory + PATH_SEPARATOR + fileName).c_str() << endl;
            return "";
        }
        int waveLength = waveFile.getWaveLength( leftBlank, rightBlank );
        double *wave = new double[waveLength];
        waveFile.getWaveBuffer( wave, leftBlank, rightBlank, waveLength );

        // 音量の正規化を行う．
        long index = (long)(44100.0 * fixedLength / 1000.0);
        double sum1 = 0.0;
        double sum2 = 0.0;
        // 固定長終了位置の音量を得る．
        for( int i = index - 1024; 0 <= i && i < index + 1024 && i < waveLength; i++ ){
            sum1 += wave[i] * wave[i];
        }
        // 左ブランク終了位置の音量を得る．
        for( int i = 0; i < 2048 && i < waveLength; i++ ){
            sum2 += wave[i] * wave[i];
        }
        // 大きい方が正規化のための音量．
        sum1 = max( sum1, sum2 );
        sum1 = VOL_NORMALIZE / sqrt( (sum1 / 2048.0) );

        for( int i = 0; i < waveLength; i++ ){
            wave[i] *= sum1;
        }

        cerr << "  computing wave...." << endl;
        clock_t cl = clock();
        vConnectPhoneme phoneme;
        phoneme.computeWave( wave, waveLength, 44100, 2.0 );
        cerr << "  Done. Elapsed time = " << (clock() - cl) << "[ms]" << endl;

        fileName = destinationDirectory + vvdName;
        if( phoneme.writePhoneme( fileName.c_str() ) ){
            cerr << "Wrote file : " << fileName << endl;
        }else{
            cerr << "Error. Failed to write : " << fileName << endl;
        }
        cerr << "====" << endl;

        delete[] wave;

        return line;
    }
};

#endif
