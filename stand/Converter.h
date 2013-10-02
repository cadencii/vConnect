/*
 * Converter.h
 * Copyright © 2010-2012 HAL, 2012 kbinani
 *
 * This file is part of vConnect-STAND.
 *
 * vConnect-STAND is free software; you can redistribute it and/or
 * modify it under the terms of the GPL License.
 *
 * vConnect-STAND is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef __Converter_h__
#define __Converter_h__

#include <time.h>
#include <sstream>
#include <boost/filesystem.hpp>
#include "TextInputStream.h"
#include "TextOutputStream.h"
#include "vConnectPhoneme.h"
#include "StringUtil.h"
#include "Task.h"
#include "WaveBuffer/WaveBuffer.h"
#include "./utau/Oto.h"

namespace vconnect
{
    /**
     * UTAU 音源を vConnect-STAND 用の音源に変換するタスク
     */
    class Converter : public Task
    {
    public:
        Converter( RuntimeOption option )
            : Task( option )
        {
        }

        ~Converter()
        {
        }

        /**
         * UTAU 音源形式を STAND 音源形式へ変換します． Unicode に対応していない簡易版です．
         * @param otoIni 分析する音源の oto.ini へのパス．
         * @param dstDir 出力先ディレクトリ名．
         * @returns 分析に成功した場合true，それ以外はfalseを返します．
         */
        void run()
        {
            namespace fs = boost::filesystem;

            string const root_oto_ini = Path::getFullPath(this->option.getInputPath());
            string const root_oto_ini_directory = Path::getDirectoryName(root_oto_ini) + Path::getDirectorySeparator();
            string encoding = this->option.getEncodingOtoIni();
            std::shared_ptr<UtauDB> db = std::make_shared<UtauDB>(root_oto_ini, encoding);
            string const destination_directory = this->option.getOutputPath();

            copy(root_oto_ini_directory, destination_directory, string("prefix.map"));
            copy(root_oto_ini_directory, destination_directory, string("character.txt"));

            Oto const* const root_oto = db->getRootOto();
            convert(root_oto, destination_directory);

            for (size_t i = 0; i < db->getSubDirectorySize(); ++i) {
                Oto const* oto = db->getSubDirectoryOto(i);
                string const oto_ini = Path::getFullPath(oto->getOtoIniPath());
                string const oto_ini_directory = Path::getDirectoryName(oto_ini);
                string sub_directory_name = oto_ini_directory.find(root_oto_ini_directory) == 0
                    ? oto_ini_directory.substr(root_oto_ini_directory.length())
                    : "";
                string destination_sub_directory = Path::combine(destination_directory, sub_directory_name);

                boost::system::error_code error;
                if (fs::create_directory(destination_sub_directory, error)) {
                    convert(oto, destination_sub_directory);
                }
            }
        }

    private:
        void copy(string const& source_directory, string const& destination_directory, std::string const& filename)
        {
            namespace fs = boost::filesystem;

            boost::system::error_code error;
            string source = Path::combine(source_directory, filename);
            string destination = Path::combine(destination_directory, filename);
            if (fs::is_regular_file(source, error) && !fs::is_regular_file(destination)) {
                fs::copy_file(source, destination, error);
            }
        }

        void convert(Oto const* const oto, string const& destination_directory)
        {
            string otoIni = oto->getOtoIniPath();
            string sourceDirectory = Path::getDirectoryName(otoIni);
            int count = 0;

            string encoding = this->option.getEncodingOtoIni();
            TextInputStream reader(otoIni, encoding);
            string destinationOtoIni = Path::combine(destination_directory, "oto.ini");
            TextOutputStream writer(destinationOtoIni, encoding, "\x0D\x0A");
            while (reader.ready()) {
                string buffer = reader.readLine();
                if( buffer.length() == 0 ){
                    continue;
                }
                count++;

                string line = this->processRecord(buffer, count, sourceDirectory, destination_directory);

                if (line.length() > 0) {
                    writer.writeLine(line);
                }
            }
            writer.close();
            reader.close();
        }

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
            vector<string> splitted = StringUtil::explode( "=", record, 2 );
            string fileName = splitted[0];
            string parametersString = splitted[1];

            vector<string> parameters = StringUtil::explode( ",", parametersString );
            string alias = parameters[0];
            float leftBlank = atof( parameters[1].c_str() );
            float fixedLength = atof( parameters[2].c_str() );
            float rightBlank = atof( parameters[3].c_str() );
            float preUtterance = atof( parameters[4].c_str() );
            float voiceOverlap = atof( parameters[5].c_str() );

            ostringstream buf;
            buf << count << ".vvd";
            string vvdName = buf.str();
            string line = vvdName + "=" + parametersString;

            cerr << "====" << endl;
            cerr << "Begin analysis : " << alias << " @ " << fileName << endl;

            WaveBuffer waveFile;
            string waveFilePath = Path::combine( sourceDirectory, fileName );
            if( waveFile.readWaveFile( waveFilePath ) != 1 ){
                cout << "error ; can't open the file, " << waveFilePath << endl;
                return "";
            }
            int waveLength = waveFile.getWaveLength( leftBlank, rightBlank );
            std::auto_ptr<double> wave_ptr(new double[waveLength]);
            double * wave = wave_ptr.get();
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

            string vvdFilePath = Path::combine( destinationDirectory, vvdName );
            if( phoneme.writePhoneme( vvdFilePath.c_str() ) ){
                cerr << "Wrote file : " << vvdFilePath << endl;
            }else{
                cerr << "Error. Failed to write : " << vvdFilePath << endl;
            }
            cerr << "====" << endl;

            return line;
        }
    };
}
#endif
