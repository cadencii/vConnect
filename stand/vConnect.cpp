/*
 *
 *    vConnect.cpp
 *                        (c) HAL 2010-
 *
 *  This files is a part of v.Connect.
 * vConnect class is a main class that connects UTAU and WORLD.
 * It is consisted of spectral, pitch and dynamics calculation.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#include "vConnect.h"
#include "stand.h"
#include <time.h>

#include <vorbis/vorbisfile.h>

#include "vConnectPhoneme.h"
#include "vConnectUtility.h"

#define TRANS_MAX 4096
double temporary1[TRANS_MAX];
double temporary2[TRANS_MAX];
double temporary3[TRANS_MAX];

double vConnect::mNoteFrequency[NOTE_NUM];
double vConnect::mVibrato[VIB_NUM];

__stnd_thread_start_retval __stnd_declspec synthesizeFromList( void *arg );

struct vConnectData {
    vConnectPhoneme *phoneme;
    int index;
    double morphRatio;
};

struct vConnectFrame {
    list<vConnectData *> dataList;
};

struct vConnectArg {
    double *f0;
    double *wave;
    double *dynamics;
    int beginFrame;
    int endFrame;
    int waveLength;
    int fftLength;
    vector<vConnectFrame *> *data;
    vector<vConnectPhoneme *> *phonemes;
    vsqEventList *eventList;
};

struct vorbisFile {
    OggVorbis_File ovf;
    int pos;
};

vConnect::vConnect()
{
    for( int i = 0; i < NOTE_NUM; i++ )
    {
        mNoteFrequency[i] = A4_PITCH * pow( 2.0, (double)(i - A4_NOTE) / 12.0 );
    }
    mVibrato[0] = 0.0;
    for( int i = 1; i < VIB_NUM; i++ )
    {
        double period = exp( 5.24 - 1.07e-2 * i ) * 2.0 / 1000.0;
        mVibrato[i] = 2.0 * ST_PI / period;
    }

    time_t timer;
    time( &timer );
    srand( (unsigned int)timer );
    mFluctTheta = 2.0 * (double)rand() / (double)RAND_MAX * ST_PI;
}

vConnect::~vConnect()
{
    for( unsigned int i = 0; i < mManagerList.size(); i++ )
    {
        SAFE_DELETE( mManagerList[i] );
    }
}

double vConnect::getPitchFluctuation( double second )
{
    double result = 1.0 + ( sin( 12.7 * ST_PI * second ) + sin ( 7.1 * ST_PI * second ) + sin( 4.7 * ST_PI * second ) / 3.0 ) / 300.0;

    return result;
}

void vConnect::emptyPath( double secOffset, string_t output )
{
    waveFileEx wave;
    wave.setOffset( secOffset );
    string toutput;
    mb_conv( output, toutput );
    wave.writeWaveFile( toutput );
    return;
}

void calculateFrameData(vector<vConnectFrame *> &dst, vector<vConnectPhoneme *> &phonemes, vsqFileEx &vsq, vector<corpusManager *> &managers, int beginFrame)
{
    vsqEventEx *itemPrev = NULL;
    for(int i = 0; i < vsq.events.eventList.size(); i++) {
        vsqEventEx *itemThis = vsq.events.eventList[i], *itemNext = (itemThis->isContinuousBack) ? vsq.events.eventList[i+1] : NULL;
        string_t lyric = itemThis->lyricHandle.getLyric();
        corpusManager::phoneme *p = managers[itemThis->singerIndex]->getPhoneme(lyric);
        double vel = pow(2.0, (double)(64 - itemThis->velocity) / 64);

        if(!p || !p->isValid){ continue; }

        vConnectPhoneme *phoneme = p->p;
        bool newPhoneme = true;
        for(int j = 0; j < phonemes.size(); j++)
        {
            if(phonemes[j] == phoneme) {
                newPhoneme = false;
                break;
            }
        }
        if(newPhoneme) {
            phonemes.push_back(phoneme);
        }
        int nextBeginFrame;
        if(itemNext) {
            if(itemPrev) {
                nextBeginFrame = max(itemNext->beginFrame, itemPrev->endFrame);
            } else {
                nextBeginFrame = itemNext->beginFrame;
            }
        }

        for(int j = itemThis->beginFrame, index = itemThis->beginFrame - beginFrame; j < itemThis->endFrame; j++, index++) {
            int frameIndex = (j - itemThis->beginFrame) * vel;
            if(dst[index] == NULL) {
                dst[index] = new vConnectFrame;
            }
            vConnectData *data = new vConnectData;
            frameIndex = max(2, min(frameIndex, itemThis->utauSetting.msFixedLength / framePeriod));
            data->index = frameIndex;
            data->phoneme = phoneme;
            if(itemThis->isContinuousBack && nextBeginFrame < j) {
                data->morphRatio = (double)(itemThis->endFrame - j) / (double)(itemThis->endFrame - nextBeginFrame);
            } else {
                data->morphRatio = 1.0;
                for(list<vConnectData *>::iterator itr = dst[index]->dataList.begin(); itr != dst[index]->dataList.end(); itr++) {
                    data->morphRatio -= (*itr)->morphRatio;
                }
            }
            dst[index]->dataList.push_back(data);
        }
        itemPrev = itemThis;
    }
}

bool vConnect::synthesize( string_t input, string_t output, runtimeOptions options )
{
#ifdef _DEBUG
    cout << "vConnect::synthesize; calling vsq.readVsqFile...";
#endif
    // 読み込みこけたら帰る
    if( false == mVsq.read( input, options ) )
    {
#ifdef _DEBUG
        cout << "vConnect::synthesize; calling vsq.readVsqFile...done, failed";
#endif
        return false;
    }
#ifdef _DEBUG
    cout << "vConnect::synthesize; calling vsq.readVsqFile...done, successed";
#endif

    // 空のときは空の wave を出力して終了
    if( mVsq.events.eventList.empty() && UtauDB::dbSize() == 0 )
    {
        emptyPath( mVsq.getEndSec(), output );
        return true;
    }

    long beginFrame, frameLength, waveLength;
    int  fftLength, aperiodicityLength;
    double *f0, *dynamics;

    double *wave;

    vector<string_t> analyze_list;
    for( int i = 0; i < UtauDB::dbSize(); i++ )
    {
        corpusManager *p = new corpusManager;
        p->setUtauDB( UtauDB::dbGet( i ), options );
        analyze_list.clear();
        for( int j = 0; j < mVsq.events.eventList.size(); j++) {
            if( mVsq.events.eventList[j]->singerIndex == i ) {
                analyze_list.push_back( mVsq.events.eventList[j]->lyricHandle.getLyric() );
            }
        }
        p->analyze( analyze_list );
        mManagerList.push_back( p );
    }

    // 準備１．先行発音などパラメータの適用，及びコントロールカーブをフレーム時刻へ変換
    this->calculateVsqInfo();
#if defined( _DEBUG )
    cout << "vConnect::synthesize; calling mVsq.dumpMapIDs..." << endl;
    mVsq.dumpMapIDs();
    cout << "vConnect::synthesize; calling mVsq.dumpMapIDs...done" << endl;
#endif
    
    aperiodicityLength = fftLength = getFFTLengthForStar(fs);

    // 準備２．合成に必要なローカル変数の初期化
    beginFrame = mVsq.events.eventList[0]->beginFrame;
    frameLength = mEndFrame - beginFrame;
    waveLength = frameLength * framePeriod * fs / 1000;

    wave = new double[waveLength];
    memset(wave, 0, sizeof(double) * waveLength);
    f0 = new double[frameLength];
    dynamics = new double[frameLength];

    // 準備３．振幅・基本周波数・時刻 t を計算する．
    this->calculateF0( f0, dynamics );

    // 準備４．合成時刻に必要な情報を整理．
    vector<vConnectFrame *> data( frameLength );
    for( int i = 0; i < frameLength; i++ )
    {
        data[i] = NULL;
    }
    vector<vConnectPhoneme *> phonemes;
    calculateFrameData(data, phonemes, mVsq, mManagerList, beginFrame);

    // 実際の合成．
    vConnectArg arg;
    arg.data = &data;
    arg.phonemes = &phonemes;
    arg.beginFrame = 0;
    arg.endFrame = frameLength;
    arg.f0 = f0;
    arg.dynamics = dynamics;
    arg.fftLength = fftLength;
    arg.wave = wave;
    arg.waveLength = waveLength;
    arg.eventList = &(mVsq.events);
    printf("begin synthesis..\n");
    clock_t cl = clock();
    synthesizeFromList(&arg);
    printf("Done: elapsed time = %f[s] for %f[s]'s synthesis.\n", (double)(clock() - cl) / CLOCKS_PER_SEC, framePeriod * frameLength / 1000.0); 

    // ファイルに書き下す．
    waveFileEx waveFile;
    waveFile.setWaveBuffer(wave, waveLength);
    waveFile.normalize();
    waveFile.setOffset((double)beginFrame * framePeriod / 1000.0);
    string str_output;
    mb_conv( output, str_output );
    waveFile.writeWaveFile( str_output );
    delete[] wave;
    delete[] f0;
    delete[] dynamics;

    return true;
}

corpusManager::phoneme* vConnect::getPhoneme(string_t lyric, int singerIndex, vector<corpusManager *> *managers)
{
    corpusManager::phoneme *ret = NULL;
    if( singerIndex < managers->size() )
    {
        ret = (*managers)[singerIndex]->getPhoneme( lyric );
    }
    return ret;
}

int getFirstItem(
    vsqEventEx **p1,
    vsqEventEx **p2, 
    corpusManager::phoneme **ph1, 
    corpusManager::phoneme **ph2,
    vsqFileEx *vsq,
    vector<corpusManager *> &managers,
    int beginFrame )
{
    int ret = vsq->events.eventList.size();
    for( int i = 0; i < vsq->events.eventList.size(); i++ )
    {
        if( vsq->events.eventList[i]->beginFrame <= beginFrame &&
            beginFrame < vsq->events.eventList[i]->endFrame )
        {
            *p1 = vsq->events.eventList[i];
            *p2 = (i + 1 < vsq->events.eventList.size()) ? vsq->events.eventList[i+1] : NULL;
            ret = i;
            if( *p1 )
            {
                *ph1 = managers[(*p1)->singerIndex]->getPhoneme((*p1)->lyricHandle.getLyric());
            }
            if( *p2 )
            {
                *ph2 = managers[(*p2)->singerIndex]->getPhoneme((*p2)->lyricHandle.getLyric());
            }
            break;
        }
    }
    return ret;
}

int calculateMelCepstrum( float *dst, int fftLength, list<vConnectData *> &frames )
{
    int ret = 0;
    memset( dst, 0, sizeof( float ) * fftLength );
    list<vConnectData *>::iterator i;
    for( i = frames.begin(); i != frames.end(); i++ )
    {
        int length;
        float *data;
        data = (*i)->phoneme->getMelCepstrum( (*i)->index, &length );
        for( int j = 0; j < length; j++ )
        {
            dst[j] += (*i)->morphRatio * data[j];
        }
        ret = max( ret, length );
    }
    return ret;
}

void calculateResidual(double *dst, int fftLength, list<vConnectData *> &frames, map_t<vConnectPhoneme *, OggVorbis_File *> &vorbisMap)
{
    memset(dst, 0, sizeof(double) * fftLength);
    float **pcm_channels;
    for(list<vConnectData *>::iterator i = frames.begin(); i != frames.end(); i++) {
        int count = 0;
        map_t<vConnectPhoneme *, OggVorbis_File *>::iterator itr = vorbisMap.find((*i)->phoneme);
        if(itr != vorbisMap.end()) {
            if(ov_pcm_seek_lap(itr->second, (*i)->index * fftLength)) {
                // シークに失敗
                continue;
            }
            while(count < fftLength) {
                int bitStream;
                long samples = ov_read_float(itr->second, &pcm_channels, fftLength - count, &bitStream);
                // 読み込み失敗．
                if(samples <= 0){ break; }
                for(int j = 0, k = count; j < samples && k < fftLength; j++, k++) {
                    dst[k] += pcm_channels[0][j] * (*i)->morphRatio;
                }
                count += samples;
            }
        }
    }
}

__stnd_thread_start_retval __stnd_declspec synthesizeFromList( void *arg )
{
    vConnectArg *p = (vConnectArg *)arg;

    // 波形の復元時に FFTW を使う上で必要なメモリの確保．
    fftw_complex *spectrum = new fftw_complex[p->fftLength];
    fftw_complex *cepstrum = new fftw_complex[p->fftLength];
    fftw_complex *residual = new fftw_complex[p->fftLength];
    double *starSpec = new double[p->fftLength];
    double *impulse  = new double[p->fftLength];
    float *melCepstrum = new float[p->fftLength];
    int cepstrumLength;

    // この処理はスレッドセーフでない．
#ifdef STND_MULTI_THREAD
    if( hMutex )
    {
        stnd_mutex_lock( hMutex );
    }
#endif
    fftw_plan forward = fftw_plan_dft_1d( p->fftLength, spectrum, cepstrum, FFTW_FORWARD,  FFTW_ESTIMATE);
    fftw_plan forward_r2c = fftw_plan_dft_r2c_1d(p->fftLength, starSpec, residual, FFTW_ESTIMATE);
    fftw_plan inverse = fftw_plan_dft_1d(p->fftLength, cepstrum, spectrum, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_plan inverse_c2r = fftw_plan_dft_c2r_1d(p->fftLength, spectrum, impulse, FFTW_ESTIMATE);
#ifdef STND_MULTI_THREAD
    if( hMutex )
    {
        stnd_mutex_unlock( hMutex );
    }
#endif

    // 検索用ハッシュ
    map_t<vConnectPhoneme *, OggVorbis_File *> vorbisMap;
    for( int i = 0; i < p->phonemes->size(); i++ )
    {
        if( !(*(p->phonemes))[i] )
        {
            continue;
        }
        OggVorbis_File *ovf = new OggVorbis_File;
        if( (*(p->phonemes))[i]->vorbisOpen( ovf ) )
        {
            vorbisMap.insert( make_pair( (*(p->phonemes))[i], ovf ) );
        } 
        else
        {
            delete ovf;
        }
    }
    //================================================================================================= ↑前処理

    int currentPosition, currentFrame = p->beginFrame;
    double currentTime = 0.0, T;

    // 合成処理
    while( currentFrame < p->endFrame )
    {
        if(p->f0[currentFrame] < 0) {
            currentFrame++;
            currentTime = (double)currentFrame * framePeriod / 1000.0;
            continue;
        }
        p->f0[currentFrame] = (p->f0[currentFrame] == 0.0) ? DEFAULT_F0 : p->f0[currentFrame];

        /* ToDo : MelCepstrum の合成結果を melCepstrum に書き込む．
                  残差波形の合成結果を starSpec に書き込む．      */
        list<vConnectData *> *frames = &(*(p->data))[currentFrame]->dataList;
        if( NULL == frames )
        {
            currentFrame++;
            currentTime = (double)currentFrame * framePeriod / 1000.0;
            continue;
        }
        cepstrumLength = 
            calculateMelCepstrum( melCepstrum, 
                                 p->fftLength, 
                                 *frames );
        calculateResidual( starSpec, 
                          p->fftLength, 
                          *frames, 
                          vorbisMap );

        // starSpec -> residual DFT を実行する．
        fftw_execute(forward_r2c);
        // メルケプストラムをstraSpecに展開．
        vConnectUtility::extractMelCepstrum(
            starSpec,
            p->fftLength,
            melCepstrum,
            cepstrumLength,
            spectrum,
            impulse,
            inverse_c2r,
            fs );
        // 合成パワースペクトルから最小位相応答を計算．
        getMinimumPhaseSpectrum(
            starSpec,
            spectrum,
            cepstrum,
            p->fftLength,
            forward,
            inverse );

        // 励起信号スペクトルと周波数領域での掛け算．
        for(int k = 0; k <= p->fftLength / 2; k++)
        {
            double real = spectrum[k][0] * residual[k][0] - spectrum[k][1] * residual[k][1];
            double imag = spectrum[k][1] * residual[k][0] + spectrum[k][0] * residual[k][1];
            spectrum[k][0] = real;
            spectrum[k][1] = imag;
        }

        // 実波形に直す．
        fftw_execute(inverse_c2r);
        currentPosition = currentTime * fs;
        for( int k = 0; k < p->fftLength / 2 && currentPosition < p->waveLength; k++, currentPosition++ )
        {
            p->wave[currentPosition] += impulse[k] * p->dynamics[currentFrame];
        }

        T = 1.0 / p->f0[currentFrame];
        currentTime += T;
        currentFrame = currentTime * 1000.0 / framePeriod;
    }

    //================================================================================================= ↓後処理
    map_t<vConnectPhoneme *, OggVorbis_File*>::iterator i;
    for( i = vorbisMap.begin(); i != vorbisMap.end(); i++) {
        ov_clear( i->second );
        delete i->second;
    }

    delete[] melCepstrum;
    delete[] impulse;
    delete[] starSpec;
    delete[] residual;
    delete[] cepstrum;
    delete[] spectrum;

    // この処理はスレッドセーフでない．
#ifdef STND_MULTI_THREAD
    if( hMutex )
    {
        stnd_mutex_lock( hMutex );
    }
#endif
    fftw_destroy_plan( forward );
    fftw_destroy_plan( forward_r2c );
    fftw_destroy_plan( inverse );
    fftw_destroy_plan( inverse_c2r );
#ifdef STND_MULTI_THREAD
    if( hMutex )
    {
        stnd_mutex_unlock( hMutex );
    }
#endif

#ifdef STND_MULTI_THREAD
#ifndef USE_PTHREADS
    //_endthreadex( 0 );
    return NULL;
#endif
    return NULL;
#endif
}

void vConnect::calculateVsqInfo( void )
{
    // 書きづらいので
    vector<vsqEventEx *> *events = &(mVsq.events.eventList);
    string_t temp;
    //vector<UtauDB*> *pDBs = this->vsq.getVoiceDBs();
    UtauDB* voiceDB;

    float msPreUtterance, msVoiceOverlap;
    int singerIndex = 0;

    mEndFrame = 0;

    /////////
    // 前から後ろをチェック
    for( unsigned int i = 0; i < events->size(); i++ )
    {
        vsqEventEx *itemi = mVsq.events.eventList[i];
        
        // タイプ判定
        while( itemi->type == _T( "Singer" ) )
        {
            // 歌手なら歌手番号拾ってきて
            singerIndex = mVsq.getSingerIndex( itemi->iconHandle.getIDS() );

            // 自分を消して
            vector<vsqEventEx*>::iterator it = events->begin();
            int j = 0;
            while( it != events->end() )
            {
                if( itemi == (*it) ) break;
                j++;
                it++;
            }
            if( it != events->end() )
            {
                events->erase( it );
                SAFE_DELETE( itemi );
            }

            // （ i 番目今消しちゃったから次に進んでるのと一緒だから ）
            if( i >= events->size() )
            {
                break;
            }
            // 次の音符へ
            itemi = mVsq.events.eventList[i];
        }
        if( singerIndex < 0 || singerIndex >= UtauDB::dbSize() )
        {
            continue;
        }
        voiceDB = UtauDB::dbGet( singerIndex );
        // 原音設定の反映
        temp = itemi->lyricHandle.getLyric();
        msPreUtterance = itemi->utauSetting.msPreUtterance;
        msVoiceOverlap = itemi->utauSetting.msVoiceOverlap;
        voiceDB->getParams( itemi->utauSetting, temp );
        itemi->utauSetting.msPreUtterance = msPreUtterance;
        itemi->utauSetting.msVoiceOverlap = msVoiceOverlap;

        // 空白文字が存在したときはVCV音素片
        itemi->isVCV = ( temp.find( _T(" ") ) != string_t::npos );

        // 休符の文字はとりあえず 'R', 'r' を対象にしてUTAUパラメタを初期化しておこう．
        itemi->isRest = ( temp.compare( _T("R") ) == 0 || temp.compare( _T("r") ) == 0);
        if(itemi->isRest){
            itemi->utauSetting.msPreUtterance = itemi->utauSetting.msVoiceOverlap = 0.0;
        }

        // 開始位置の計算
        itemi->beginFrame = (long)( (
                        mVsq.vsqTempoBp.tickToSecond( itemi->tick ) * 1000.0 - itemi->utauSetting.msPreUtterance
                        * pow( 2.0, ( 64.0 - itemi->velocity ) / 64.0 ) ) / framePeriod );
        // ポルタメントが０％の場合適当な値を入れておく
        if( itemi->portamentoLength < 2 )
            itemi->portamentoLength = 2;
        itemi->singerIndex = singerIndex;
    }

    /////////
    // 後ろから前をチェック
    for( unsigned int i = 0; i < events->size(); i++ ){
        // まず Tick 時刻から終了時刻を計算
        (*events)[i]->endFrame = (long)(
            ( mVsq.vsqTempoBp.tickToSecond( mVsq.events.eventList[i]->tick + mVsq.events.eventList[i]->length ) * 1000.0 ) / framePeriod );

        // 一個前の音符がある場合，連続性のチェック
        if( i ){
            // i 番目の音符が，i - 1 番目の音符が終わる前に始まる場合連続とみなす
            (*events)[i-1]->isContinuousBack = ( (*events)[i]->beginFrame <= (*events)[i-1]->endFrame );

            ////* 連続時のオーバーラップの設定 */
            
            if( (*events)[i-1]->isContinuousBack )
            {
                // i 番目が CV 音素片の場合
                if( !(*events)[i]->isVCV )
                {
                    // まず i 番目の先行発音を i - 1 番目に適用する
                    (*events)[i-1]->endFrame -= (long)( (*events)[i]->utauSetting.msPreUtterance
                                                        * pow( 2.0, ( 64.0 - (*events)[i]->velocity ) / 64.0 ) / framePeriod );
                    // さらにオーバーラップも適用する
                    (*events)[i-1]->endFrame += (long)( (*events)[i]->utauSetting.msVoiceOverlap / framePeriod );
                }
                else
                {
                    if( (*events)[i-1]->endFrame - (*events)[i-1]->beginFrame > 20 )
                    {
                        (*events)[i-1]->endFrame -= 20;
                    }
                    else
                    {
                        (*events)[i-1]->endFrame = (*events)[i-1]->beginFrame;
                    }
                }

                // 最後にセーフガードとして短くなりすぎないようにチェック
                if( (*events)[i-1]->endFrame < (*events)[i-1]->beginFrame )
                {
                    (*events)[i-1]->endFrame = (*events)[i-1]->beginFrame;
                }
            }
        }

    }

    for( unsigned int i = 0; i < events->size(); i++ )
    {
        if( mEndFrame < (*events)[i]->endFrame )
        {
            mEndFrame = (*events)[i]->endFrame;
        }
    }

    // コントロールカーブは vsq 管理クラスにやってもらう
    mControlCurves.resize( mVsq.controlCurves.size() );
    for( unsigned int i = 0; i < mControlCurves.size(); i++ )
    {
        mVsq.controlCurves[i].getList( mControlCurves[i], mVsq.getTempo() );
    }
}

void vConnect::calculateF0( double *f0, double *dynamics )
{
    double pitch_change, tmp, vibratoTheta = 0.0, vibratoRate, vibratoDepth;
    long beginFrame = mVsq.events.eventList[0]->beginFrame;
    long frameLength = mEndFrame - beginFrame;
    long index = 0;
    long portamentoBegin, portamentoLength;
	long previousEndFrame = LONG_MIN, vibratoBeginFrame = 0, noteBeginFrame;
    int  pitIndex = 0, pbsIndex = 0, dynIndex = 0; // ControlCurve Index

    for( unsigned int i = 0; i < mVsq.events.eventList.size(); i++ )
    {
        vsqEventEx *itemi = mVsq.events.eventList[i];

        // デフォルト値で埋める
        for( ; index < itemi->beginFrame - beginFrame && index < frameLength; index++ )
        {
            f0[index] = -1.0;//0.0;
            dynamics[index] = 0.0;
        }

        // 後続のノートがあるかどうか
        if( !itemi->isContinuousBack )
        {
            // ないなら，ポルタメントの開始位置を設定
            portamentoBegin = itemi->endFrame - 50;
            if( portamentoBegin < itemi->beginFrame )
            {
                portamentoBegin = itemi->beginFrame;
            }
            portamentoBegin -= beginFrame;
        }
        else
        {
            // ありえない開始位置にしておくよ！！
            portamentoBegin = LONG_MAX;
        }

		// vibrato 開始位置は元々の音符開始位置近辺から計算する．そこまで厳密じゃなくていいよね．
		if( previousEndFrame > itemi->beginFrame - beginFrame )
        {
			vibratoBeginFrame = previousEndFrame;
			vibratoBeginFrame += (long)( 1000.0 * mVsq.vsqTempoBp.tickToSecond( itemi->vibratoDelay ) /framePeriod );
		}
        else
        {
			vibratoBeginFrame = itemi->beginFrame - beginFrame;
			vibratoBeginFrame += (long)( 1000.0 * mVsq.vsqTempoBp.tickToSecond( itemi->vibratoDelay ) /framePeriod );
		}

        // ノート・ビブラート・微細振動を書く
        for( ; index < itemi->endFrame - beginFrame && index < frameLength; index++ )
        {
            // ピッチetcカーブに格納されている値の内どれを使うか？
            while( index + beginFrame > mControlCurves[PITCH_BEND][pitIndex].frameTime )
            {
                pitIndex++;
            }
            while( index + beginFrame > mControlCurves[PITCH_BEND_SENS][pbsIndex].frameTime )
            {
                pbsIndex++;
            }
            while( index + beginFrame > mControlCurves[DYNAMICS][dynIndex].frameTime )
            {
                dynIndex++;
            }
            pitch_change = pow( 2, (double)mControlCurves[PITCH_BEND][pitIndex].value / 8192.0 * (double)mControlCurves[PITCH_BEND_SENS][pbsIndex].value / 12.0 );
            f0[index] = mNoteFrequency[itemi->note] * pitch_change * getPitchFluctuation( (double)index * framePeriod / 1000.0 );
            dynamics[index] = (double)mControlCurves[DYNAMICS][dynIndex].value / 64.0;
            if( index > portamentoBegin )
            {
                dynamics[index] *= 1.0 - (double)( index - portamentoBegin ) / 50.0;
            }
            
            /* Vibrato */
            if( index > vibratoBeginFrame )
            {
				double pos = (double)(index - vibratoBeginFrame ) / (double)( itemi->endFrame - beginFrame - vibratoBeginFrame );
                vibratoRate = mVibrato[itemi->vibratoHandle.getVibratoRate( pos )];
                vibratoDepth = (double)itemi->vibratoHandle.getVibratoDepth( pos ) * 2.5 / 127.0 / 2.0;
                vibratoTheta += vibratoRate * framePeriod / 1000.0;
                f0[index] *= pow( 2.0, 1.0 / 12.0 * vibratoDepth * sin( vibratoTheta ) );
                if( vibratoTheta > 2.0 * ST_PI )
                {
                    vibratoTheta -= 2.0 * ST_PI;
                }
            }
            else
            {
                vibratoTheta = 0.0;
            }
		}
		previousEndFrame = itemi->endFrame - beginFrame;
    }

    previousEndFrame = LONG_MIN;

    // ポルタメントを描きます．（ビブラートとは実は順番依存）
    for( unsigned int i = 0; i < mVsq.events.eventList.size(); i++ )
    {
        vsqEventEx *itemi = mVsq.events.eventList[i];

        if( !itemi->isContinuousBack )
        {
            continue;
        }
        else
        {
            // 後続がいる場合は必要なパラメータを計算してポルタメントを書く
            if( previousEndFrame > itemi->beginFrame )
            {
                noteBeginFrame = previousEndFrame;
            }
            else
            {
                if( itemi->isVCV )
                {
                    noteBeginFrame = (long)( mVsq.vsqTempoBp.tickToSecond( itemi->tick ) * 1000.0 / framePeriod );
                }
                else
                {
                    noteBeginFrame = itemi->beginFrame;
                }
            }
            portamentoBegin = noteBeginFrame
                      + (long)((double)(itemi->endFrame - noteBeginFrame)
                      * (1.0 - (double)(itemi->portamentoLength) / 100.0));
            tmp = mNoteFrequency[mVsq.events.eventList[i + 1]->note] / mNoteFrequency[itemi->note];
        }

        portamentoLength = itemi->endFrame - portamentoBegin;
        double inv_portamentoLength = 1.0 / (double)portamentoLength;
        long frameOffset = portamentoBegin - beginFrame;
        for( long j = 0; j < portamentoLength && j + frameOffset < frameLength; j++ )
        {
            double x = (double)j * inv_portamentoLength;
            double portamentoChangeRate = (sin( ST_PI * 4.0 / 3.0 * x ) * (1.5 - x) / 1.5);
            f0[j + frameOffset] *= pow( tmp, 0.5 * (1.0 - cos( ST_PI * x )) - (double)itemi->portamentoDepth / 100.0 * portamentoChangeRate);
            dynamics[j + frameOffset] *= pow(tmp / fabs(tmp) * 3.0, - (double)itemi->decay / 100.0 * portamentoChangeRate);
        }
        for( long j = portamentoLength; j < portamentoLength * 3 / 2 && j + frameOffset < frameLength; j++ )
        {
            double x = (double)j * inv_portamentoLength;
            double portamentoChangeRate = (sin( ST_PI * 4.0 / 3.0 * x ) * (1.5 - x) / 1.5);
            f0[j + frameOffset] *= pow( tmp, - (double)itemi->portamentoDepth / 100.0 * portamentoChangeRate );
            dynamics[j + frameOffset] *= pow(tmp / fabs(tmp) * 3.0, - (double)itemi->attack / 100.0 * portamentoChangeRate);
        }
        previousEndFrame = itemi->endFrame;
    }
}
