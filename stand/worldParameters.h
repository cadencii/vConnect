#ifndef __worldParameters_h__
#define __worldParameters_h__

/// <summary>
/// WORLD �̕��̓p�����[�^�̈ꕔ��ێ����܂��D
/// </summary>
class worldParameters
{
public:

    worldParameters();
    ~worldParameters();

    /// <summary>
    /// �^����ꂽ�g�`���� WORLD �̕��̓p�����[�^�𒊏o���܂��D
    /// </summary>
    /// <param name="wave">�g�`���i�[�����z��D</param>
    /// <param name="waveLength">�z�񒷁D</param>
    /// <param name="fs">�W�{�����g���D</param>
    /// <param name="framePeriod">�t���[���P�ʎ��ԁD</param>
    /// <returns>���������Ƃ��� true�C���s�����Ƃ��� false ��Ԃ��܂��D</returns>
    bool computeWave(double *wave, int waveLength, int fs, double framePeriod);

    /// <summary>
    /// �ێ����Ă��镪�͍ς݃p�����[�^���t�@�C���ɏ����o���܂��D
    /// </summary>
    /// <param name="path">�������ރt�@�C���p�X�D</param>
    /// <returns>���������Ƃ��� true�C���s�����Ƃ��� false ��Ԃ��܂��D</returns>
    bool writeParameters(const char *path);

    /// <summary>
    /// ���͍ς݃p�����[�^��ǂݍ��݂܂��D
    /// </summary>
    /// <param name="path">�ǂݍ��ރt�@�C���p�X�D</param>
    /// <returns>���������Ƃ��� true�C���s�����Ƃ��� false ��Ԃ��܂��D</returns>
    bool readParameters(const char *path);

    /// <summary>
    /// ���͍ς݃p�����[�^�̊Y���ӏ���ǂݏo���܂��D
    /// </summary>
    /// <param name="f0">F0 ����������ޔz��D</param>
    /// <param name="t">���Ԏ����������ޔz��D</param>
    /// <param name="pulseLocation">�t���[���������̃p���X�ʒu���������ޔz��D</param>
    /// <param name="fs">�W�{�����g���D</param>
    /// <param name="beginFrame">�J�n�t���[�������D</param>
    /// <param name="endFrame">�I���t���[�������D</param>
    /// <param name="framePeriod">�t���[���P�ʎ��ԁD</param>
    /// <returns>���������Ƃ��� true�C���s�����Ƃ��� false ��Ԃ��܂��D</returns>
    bool getParameters(float *f0, float *t, int *pulseLocations, int fs, double beginTime, double endTime, double framePeriod);

private:

    void destroy();

    float *f0;
    float *t;
    int *pulseLocations;
    int tLen;
    float framePeriod;
};

#endif
