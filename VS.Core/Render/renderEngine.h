#ifndef RENDERENGINE_H
#define RENDERENGINE_H

#include <algorithm>
#include <fftw/fftw3.h>
#include <stdlib.h>
#include <math.h>

#ifndef PI
#define PI 3.1415926535897932384
#endif

#ifndef max
    #define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef __GNUC__
    #pragma warning( disable : 4996 )
    #pragma comment(lib, "libfftw3-3.lib")
    #pragma comment(lib, "libfftw3f-3.lib")
    #pragma comment(lib, "libfftw3l-3.lib")
#endif

class RenderEngine
{
public:
    RenderEngine();

    constexpr static auto MAX_FFT_LENGTH = 2048;
    constexpr static auto FLOOR = 71.0;
    constexpr static auto FLOOR_F0 = 71.0;
    constexpr static auto DEFAULT_F0 = 150.0;
    constexpr static auto LOW_LIMIT = 65.0;
    constexpr static auto ST_PI = (3.141592653589793238462643383279);

    // Известные в настоящее время ошибки:
    // Decimateforf0: Сразу после начала- непосредственно перед концом 4 выборки будут иметь ошибки.

    // 71 Гц - это нижний предел длины БПФ при fs: от 44100 до 2048.
    // Если он установлен на 70 Гц, потребуется 4096 точек.

    // F0 предполагаемый метод DIO : Работа распределенного встроенного фильтра
    void dio(double *x, int xLen, int fs, double framePeriod,
             double *timeAxis, double *f0);
    int getSamplesForDIO(int fs, int xLen, double framePeriod);

    // Способ оценки огибающей спектра STAR : Synchronous Technique and Adroit Restoration
    int getFFTLengthForStar(int fs);
    void star(double *x, int xLen, int fs, double *timeAxis, double *f0,
              double **specgram, bool mode);
    void starGeneralBody(double *x, int xLen, int fs, double f0, double t, int fftl,
                         double * sliceSTAR, double *waveform, double *powerSpec, fftw_complex *ySpec, fftw_plan *forwardFFT);

    // Способ оценки апериодического индекса PLATINUM.
    void platinum(int fs, double *f0, int tLen,
                  double **aperiodicity);
    void calculateAperiodicity(double *aperiodicity, int fftl, int fs, double *periodicSpec);

    // Способ оценки апериодического индекса PLATINUM from WORLD0.0.4
    void platinum_v4(double *x, int xLen, int fs, double *timeAxis, double *f0, double **specgram,
                     double **residualSpecgram);

    void getWedgeList(double *x, int xLen, int vuvNum, int *stList, int *edList, int fs, double framePeriod, double *f0, int *wedgeList);

    int getPulseLocations(double *x, int xLen, double *totalPhase, int vuvNum, int *stList, int *edList, int fs, double framePeriod, int *wedgeList, double *pulseLocations);

    void getOneFrameResidualSpec(double *x, int xLen, int fs, int positionIndex, double framePeriod, double f0, double *specgram, int fftl, double *pulseLocations, int pCount,
                                 double *residualSpec, fftw_plan *forwardFFT, fftw_complex *tmpSpec, fftw_complex *starSpec, fftw_complex *ceps, double *tmpWave,
                                 fftw_plan minForward, fftw_plan minInverse);
    // WORLD Synthesis
    void synthesis(double *f0, int tLen, double **specgram, double **aperiodicity, int fftl, double framePeriod, int fs,
                   double *synthesisOut, int xLen);
    void getMinimumPhaseSpectrum(double *inputSpec, fftw_complex *spectrum, fftw_complex *cepstrum, int fftl, fftw_plan forward, fftw_plan inverse);

    // WORLD Synthesis 0.0.4
    void synthesis_v4(double *f0, int tLen, double **specgram, double **residualSpecgram, int fftl, double framePeriod, int fs,
                      double *synthesisOut, int xLen);

    //------------------------------------------------------------------------------------
    // Matlab функции

    double std_mat(double *x, int xLen);

    void inv(double **r, int n, double **invr);

    void fftfilt(double *x, int xlen, double *h, int hlen, int fftl, double *y);

    float randn(void); // Генерация гауссовского шума

    void histc(double *x, int xLen, double *y, int yLen, int *index);

    void interp1(double *t, double *y, int iLen, double *t1, int oLen, double *y1);

    long decimateForF0(double *x, int xLen, double *y, int r);

    void filterForDecimate(double *x, int xLen, double *y, int r);

    int c_round(double x); // Округление

    void diff(double *x, int xLength, double *ans);

    // Перенос функции Matlab interp1Q.
    // В основном то же самое, но нужно явно указать количество элементов в массиве.
    void interp1Q(double x, double shift, double *y, int xLength, double *xi, int xiLength, double *ans);
};

#endif // RENDERENGINE_H
