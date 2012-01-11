/*
 * Transcriber.cpp
 * Copyright © 2011-2012 HAL, 2012 kbinani
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
#include <time.h>

#include "Transcriber.h"
#include "utauVoiceDB/UtauDB.h"
#include "vConnectPhoneme.h"
#include "vConnectUtility.h"
#include "Configuration.h"


// 転写システム
void Transcriber::run()
{
    string src_path = this->option.getInputPath();
    string dst_path = this->option.getOutputPath();
    string codepage = this->option.getEncodingOtoIni();
    UtauDB src, dst;

    src.read( src_path, codepage.c_str() );
    dst.read( dst_path, codepage.c_str() );
    map_t<string, int> analyzedItems;

    for( int i = 0; i < src.size(); i++ ){
        UtauParameter src_param, dst_param;
        src.getParams(src_param, i);
        cout << "Begin analysis : " << src_param.lyric << endl;

        map_t<string, int>::iterator itr = analyzedItems.find(src_param.fileName);
        if( itr != analyzedItems.end() ){
            cout << " Already analyzed." << endl;
            continue;
        }

        clock_t cl = clock();

        if( dst.getParams( dst_param, src_param.lyric ) != 1 ){
            cout << " error; not found : " << src_param.lyric << endl;
            continue;
        }
        if( dst_param.isWave != src_param.isWave ){
            cout << " error; conflting format : " << src_param.fileName << endl;
            continue;
        }

        if( false == dst_param.isWave ){
            // 圧縮形式のマッチング．
            vConnectPhoneme src_phoneme, dst_phoneme;

            string tmp_path;
            src.getDBPath(tmp_path);
            tmp_path += src_param.fileName;
            if( !src_phoneme.readPhoneme( tmp_path.c_str() ) ){
                cout << " error; can't read file : " << tmp_path << endl;
                continue;
            }

            dst.getDBPath(tmp_path);
            tmp_path += dst_param.fileName;
            if( !dst_phoneme.readPhoneme( tmp_path.c_str() ) ){
                cout << " error; can't read file : " << tmp_path << endl;
                continue;
            }

            _transcribe_compressed(&src_phoneme, &dst_phoneme);

/*            src.getDBPath(tmp_path);
            tmp_path += src_param.fileName;
            mb_conv(tmp_path, s);
            src_phoneme.writePhoneme(s.c_str());
*/
            dst.getDBPath(tmp_path);
            tmp_path += dst_param.fileName;
            dst_phoneme.writePhoneme( tmp_path.c_str() );
        }else{
            vConnectPhoneme src_phoneme, dst_phoneme;
            /* ここは生波形用 */
        }

        cout << "Done. Elapsed time = " << (double)(clock() - cl) / CLOCKS_PER_SEC << " [s]" << endl;

        analyzedItems.insert(make_pair(src_param.fileName, i));
    }
}

void Transcriber::_transcribe_compressed(vConnectPhoneme *src, vConnectPhoneme *dst)
{
    int src_len, dst_len;
    double *src_env, *dst_env;
    double *src_to_dst, *dst_to_src, *dst_to_src_stretched;

    src_len = src->getTimeLength();
    dst_len = dst->getTimeLength();

    src_env = new double[src_len];
    dst_env = new double[dst_len];
    src_to_dst = new double[src_len];
    dst_to_src = new double[dst_len];
    dst_to_src_stretched = new double[src_len];

    cout << " extract source compressed wave." << endl;
    _calculate_compressed_env(src_env, src, src_len);
    cout << "  done." << endl;

    cout << " extract target compressed wave." << endl;
    _calculate_compressed_env(dst_env, dst, dst_len);
    cout << "  done." << endl;

    cout << " calculate matching between two phonemes." << endl;
    cout << "   ; src length = " << src_len << ", dst length = " << dst_len << endl;

    cout << "  stretch dst->src." << endl;
    for( int i = 0; i < src_len - 1; i++ ){
        double tmp = (double)i / (double)src_len * (double)dst_len;
        if( tmp >= dst_len - 1 ){
            dst_to_src_stretched[i] = dst_env[dst_len-1];
        }else{
            dst_to_src_stretched[i] = vConnectUtility::interpolateArray(tmp, dst_env);
        }
    }
    dst_to_src_stretched[src_len-1] = dst_env[dst_len-1];

    cout << "  calculate streching function." << endl;
    vConnectUtility::calculateMatching(dst_to_src_stretched, src_to_dst, src_env, dst_to_src_stretched, src_len);

    cout << "  stretch src->dst." << endl;
    double framePeriod = Configuration::getMilliSecondsPerFrame();
    for( int i = 0; i < dst_len - 1; i++ ){
        double tmp = (double)i / (double)dst_len * (double)src_len;
        if( tmp >= src_len - 1 ){
            dst_to_src[i] = dst_to_src_stretched[src_len-1];
        }else{
            dst_to_src[i] = vConnectUtility::interpolateArray(tmp, dst_to_src_stretched) * framePeriod / 1000.0 / (double)src_len * (double)dst_len;
        }
    }
    dst_to_src[dst_len-1] = dst_to_src_stretched[src_len-1] / (double)src_len * (double)dst_len;

    for( int i = 0; i < src_len; i++ ){
        dst_to_src_stretched[i] = src_to_dst[i] * framePeriod / 1000.0;
    }
    memcpy( src_to_dst, dst_to_src_stretched, sizeof( double ) * src_len );
    cout << "  done." << endl;

    dst->setTimeAxis( dst_to_src, dst_len );
    dst->setTimeAxis( src_to_dst, src_len );

    delete[] dst_to_src_stretched;
    delete[] src_to_dst;
    delete[] dst_to_src;
    delete[] src_env;
    delete[] dst_env;
}

void Transcriber::_calculate_compressed_env( double *dst, vConnectPhoneme *src, int length )
{
    int fftl = 2048;
    double      *out = new double[fftl];
    double      *pow_spec = new double[fftl];

    double      *res_wave = new double[fftl];
    fftw_complex *res_spec = new fftw_complex[fftl];
    fftw_complex *spectrum = new fftw_complex[fftl];
    fftw_complex *cepstrum = new fftw_complex[fftl];

    fftw_plan res_forward = fftw_plan_dft_r2c_1d( fftl, res_wave, res_spec, FFTW_ESTIMATE );
    fftw_plan forward     = fftw_plan_dft_1d( fftl, spectrum, cepstrum, FFTW_FORWARD,  FFTW_ESTIMATE );
    fftw_plan inverse     = fftw_plan_dft_1d( fftl, cepstrum, spectrum, FFTW_BACKWARD, FFTW_ESTIMATE );
    fftw_plan inverse_c2r = fftw_plan_dft_c2r_1d( fftl, spectrum, out, FFTW_ESTIMATE );

    OggVorbis_File ovf;
    src->vorbisOpen( &ovf );
    float **pcm_channels;

    for( int i = 0; i < length; i++ ){
        int mel_len;
        int c;
        float *mel_cep;
        double sum = 0.0;

        // ケプストラムからパワースペクトルを計算．
        mel_cep = src->getMelCepstrum( i, &mel_len );
        vConnectUtility::extractMelCepstrum( pow_spec, fftl, mel_cep, mel_len, spectrum, out, inverse_c2r, fs );
        getMinimumPhaseSpectrum( pow_spec, spectrum, cepstrum, fftl, forward, inverse );

        // Ogg ストリームから残差波形をデコード．
        for( c = 0; c < fftl; ){
            int bitStream;
            long samples = ov_read_float( &ovf, &pcm_channels, fftl - c, &bitStream );
            if( samples <= 0 ){
                break;
            }
            for( int j = 0, k = c; j < samples && k < fftl; j++, k++ ){
                res_wave[k] = pcm_channels[0][j];
            }
            c += samples;
        }

        // 残差スペクトルの計算．
        fftw_execute( res_forward );

        for( int k = 0; k <= fftl / 2; k++ ){
            double real = spectrum[k][0] * res_spec[k][0] - spectrum[k][1] * res_spec[k][1];
            double imag = spectrum[k][1] * res_spec[k][0] + spectrum[k][0] * res_spec[k][1];
            spectrum[k][0] = real;
            spectrum[k][1] = imag;
        }

        fftw_execute( inverse_c2r );

        for( int j = 0; j < fftl; j++ ){
            sum += out[j] * out[j];
        }
        dst[i] = sum;
    }

    fftw_destroy_plan( inverse_c2r );
    fftw_destroy_plan( forward );
    fftw_destroy_plan( inverse );
    fftw_destroy_plan( res_forward );

    src->vorbisClose( &ovf );
    delete[] spectrum;
    delete[] cepstrum;
    delete[] res_spec;
    delete[] res_wave;
    delete[] pow_spec;
    delete[] out;
}

