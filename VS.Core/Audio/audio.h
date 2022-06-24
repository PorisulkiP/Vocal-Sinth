#ifndef AUDIO_H
#define AUDIO_H

#include <fstream>
#include <cstdio>
#include <cmath>
#include <complex>
#include <limits>
#include <string>
#include <thread>
#include <future>
#include <mutex>
#include <float.h>
#include <io.h>

#include <QTextDecoder>
#include <QString>
#include <QThread>
#include <QFile>

#include "Format/wave.h"
#include "variables.h"
#include "musicMath.h"
#include "voicebank.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <AL/alext.h>

#include "fftw/fftw3.h"

#if defined (_MSC_VER)
__pragma(warning (push))
    __pragma(warning (disable : 4244))
    __pragma(warning (disable : 4457))
    __pragma(warning (disable : 4458))
    __pragma(warning (disable : 4389))
    __pragma(warning (disable : 4996))
#elif defined (__GNUC__)
_Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wconversion\"")
    _Pragma("GCC diagnostic ignored \"-Wsign-compare\"")
    _Pragma("GCC diagnostic ignored \"-Wshadow\"")
#endif

using namespace std;

class Audio: public QObject
{
    Q_OBJECT
public:
    Audio();
    Audio(const string &path);
    ~Audio();

    // Буфер, содержащий либо оба канала с чередованием (в случае стерео), либо только один канал.
    ALuint getALBufferAll(void) { return stereo ? alBuffer : alBufferLeft; }

    // буфер содержащий только левый канал
    ALuint getALBufferLeft(void) { return alBufferLeft; }

    // буфер содержащий только левый канал или 0, если звук одноканальный
    ALuint getALBufferRight(void) { return alBufferRight; }

    // Стерео ли звучание
    bool getStereo (void) { return stereo; }
    template<class T>
    void setBuffers(vector<T> &data);

    bool openFile(const string &path);
    void play(); // воспроизводим аудио
    void pause(); // останавливаем проигрывание
    void stop(); // возвращаемся в начало файла
    void update(); // обновление аудио
    void seekTo(float seek_to); // устанавливаем место, с которого проигрывается файл
    void cut(); // обрезаем аудио
    void connect(); // соединяем с другим аудиофалом
    void deleteWav(); // удаляем аудиофайл из памяти
    void reportError();
    void mute(); // Отключение звука в буфере

    double duration() const     { return _duration;         }
    int samplesCount() const    { return samples_count;     }
    short numChannels() const   { return header.numChannels;}
    short depth() const         { return bits;              }
    auto data16bit() const      { return _data16bit;        }
    auto *samples() const       { return &_LSamples;        }
    auto *LSamples()            { return &_LSamples;        }
    auto *RSamples()            { return &_RSamples;        }
    auto *LDecibels()           { return &_DbL;             }
    auto *RDecibels()           { return &_DbR;             }
    bool isPlaying();

    void setOptimazer(const double opt){
        optimizer = opt;
        readSamples(_dataChar);
    }

    ALenum toAlFormat(short samples);
    ALenum toAlMonoFormat(short samples);

    // Специальные функции для работы с нотами
    // Подстраивание буфера под длинну ноты
    void setNewLength(int newLength);
    void setNewTone(int newTone);
    void setNewVolume(int newVol); // изменение громкости у НОТЫ

    float getVolume();

    string getFileName() const;
    void setFileName(const string &newFileName);

    Voicebank *getVB() const { return VB; }
    void setVB(Voicebank *newVB) { VB = newVB; }

    ALuint getAlBuffer() const;
    void setAlBuffer(ALuint newAlBuffer);

    ALuint source() const;
    void setSource(ALuint newSource);

    bool haveChange() const;
    void setHaveChange(bool newHaveChange);

signals:
    void dataUpdeted();

public slots:
    void setVolume(int volume); // изменение громкости у ПАРТИИ
    void setMove(int X); // перемещаем в пространстве источник звука

private:
    bool _haveChange = false;
    int id;
    int circle = 0;
    Voicebank *VB= nullptr;
    Oto* oto = nullptr; // oto ноты
    void fftShift(vector<double> &plan);
    void notePreparation();
    // Объявляется структура заголовка WAV файла
    wavHeader header;
    chunk_t chunk;

    string fileName;
    bool getWaveData(); // получаем все данные об аудио файле

    vector<char> readFile();
    vector<short> readINT16File();
    vector<float> readFloatFile();

    // Объявляется временный массив для ноты
    vector<short> tmpChar;
    vector<short> tmpShort;
    vector<short> tmpFloat;

    // Преобразование данных
    vector<short> floatTo16bit(vector<float> &_buffer);
    vector<short> charTo16bit(vector<char> &_buffer);
    vector<char> floatToChar(vector<float> &_buffer);
    vector<char> int16ToChar(vector<short> &_buffer);

    void readSamples(const vector<char> data);

    vector<char> _LSamples; // семплы, в линейном виде, левого канала
    vector<char> _RSamples; // семплы, в линейном виде, правого канала

    QList<float> _DbL; // Децибелы левого канала
    QList<float> _DbR; // Децибелы правого канала

    vector<float> _freq; // частота ноты

    // Отключён ли звук у занного аудио
    bool muted = false;

    FILE* fin;
    string _path;

    double optimizer = 2.0; // Значение от 1.0 до 2.0
    double _duration = 0.0;
    int sampwidth = 1;
    float peak = 0, k = 0;
    int samples_count = 0;
    int sample_size = 0;

    vector<char> pcm_charBuffer;
    vector<short> pcm_16buffer;

    // Данные из аудиофайла
    vector<char> _dataChar;
    vector<short> _data16bit;
    vector<float> _datafloat;

    // Буфер, содержащий чередующиеся левый и правый каналы, используемый для воспроизведения стерео.  Если ресурс не используется, он не используется.
    ALuint alBuffer;

    // Буфер, содержащий только левый канал, или моноканал в случае моноресурса.
    ALuint alBufferLeft;

    // Буфер, содержащий только правый канал, не используется, если ресурс не стерео.
    ALuint alBufferRight;

    // Стерео ли звук
    bool stereo;
    // Обработана ли уже нота (обрезана по offset, наден ли oto)
    bool isReady = false;

    ALuint          _source;
    ALfloat         gain = 1;
    ALfloat         tmp_gain = 1;
    ALint           size, freq;
    ALshort         bits;
    ALenum          format;
    ALint           state = AL_STOPPED;
    ALboolean       mloop = AL_FALSE;
    // Ориентация слушателя. (Первые 3 элемента – направление «на», последние 3 – «вверх»)
    ALfloat         ListenerOri[6] = { 0, 0, -1,  0, 1, 0 };

    mutex _mutex;
    int maxThreads = thread::hardware_concurrency();
};

enum wavChunks {
    RiffHeader = 0x46464952,
    WavRiff = 0x54651475,
    Format = 0x020746d66,
    LabeledText = 0x478747C6,
    Instrumentation = 0x478747C6,
    Sample = 0x6C706D73,
    Fact = 0x47361666,
    Data = 0x61746164,
    Junk = 0x4b4e554a,
};


#endif
