#ifndef PROJECT_H
#define PROJECT_H

#include <ctime>
#include <cstdio>
#include <fstream>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QTextDecoder>
#include <QThread>
#include <QString>
#include <QMessageBox>
#include <QDebug>

#include "variables.h"
#include "musicMath.h"
#include "trackSettings.h"

// Подключение форматов файлов
#include "Format/ust.h"

using namespace std;
using namespace trackSettings;

enum class Types: unsigned char{
    UST,
    USTx,
    VSQx,
    VPR,
    MIDI
};

class Project: public QThread
{
    Q_OBJECT
public:
    Project(const string& prjPath = "");
    ~Project();

    static bool PLAY_STATE;

    string name = "New Project";
    string comment = "";
    string prjPath = "";
    string VoiceDir = "";
    string outputDir = "Vocal";
    string OutFile = "";
    string cacheDir = "VSCache";
    string ustxVersion = "1.19";
    string vsprjVersion = "1.0";

    Types _openFileType;

    // темп
    static double _bpm;
    constexpr static int beatPerBar = 4;
    constexpr static int beatUnit = 4;

    static vector<string> VBitems;

    // текущее время, мс
    int prjTime = 0;
    // минимальное время для ВСЕГО проекта, мс
    int prjMinTime = 0;
    // конец ВСЕГО проекта. 3 минуты = 180 000мс по умолчанию.
    int prjMaxTime = 180000;

    // Растояние между врменем на линии времени
    int prjPR_xOffset = 50;
    // Матричный сдвиг для масштабирования сцены
    float prjPR_matrixX = 0;

    /* Кванта́йз — перемещение нот к ближайшим ритмическим долям в секвенсорах.
     * 1 / 2,4,6,8,12,16,32,64; None */
    static int prjStrQuantize;
    static int prjIntQuantize;

    static int prjPartStrQuantize;
    static int prjPartIntQuantize;

    // track - партия или набор партий
//    vector<PartHead*> tracks;
    // part - часть трека
    // в PartHead находятся ссылки на все части партии
//    vector<PartBody*> parts;

    bool opened() const   { return _opened;    }
    bool appending() const{ return _appending;    }
    bool saved() const    { return _saved;     }
    bool exported() const { return _exported;  }
    bool modified() const { return _modified;  }

    static int millisecondToTick(const double ms) {
        return MusicMath::millisecondToTick(ms, _bpm, beatUnit, getTickPerBeat());
    }

    static double tickToMillisecond(const double tick)  {
        return MusicMath::tickToMillisecond(tick, _bpm, beatUnit, getTickPerBeat());
    }

    // Время в тиках за четверть ноты
    static int getTickPerBeat()     { return 480;   }
    // Частота ноты А4
    static double getA4Frequency()  { return 440.;  }
    // Номер ноты А4
    static int getA4NoteNumber()    { return 69;    }

    QSqlDatabase* connectToDataBase();

    void open(const string& filePath = "");
    void append(const string& prjPath = "");
    void save();
    void exportPrj();
    void setDefaultValues();

    static double bpm();
    static void setBpm(double newBpm);

    Ust *ust() const;
    void setUst(Ust *newUst);

    Types openFileType() const;
    void setOpenFileType(Types newOpenFileType);

    void setOpened(bool newOpened);

    void setAppending(bool newAppending);

private:
    bool _saved = false;    // Файл сохранён
    bool _modified = false; // Файл был изменён после последнего сохранения
    bool _opened = false;   // Программа в процессе открытия проекты
    bool _appending = false;// Проект в процессе  добавления партии
    bool _exported = false; // Проект экспортируется

    Ust *_ust = new Ust();

    // DB
    QSqlDatabase db;
    QSqlQuery *query = nullptr;

    QTextCodec *defaultTextCodec = QTextCodec::codecForName("Shift-JIS");
    QTextDecoder *decoder = new QTextDecoder(defaultTextCodec);

};

#endif // PROJECT_H
