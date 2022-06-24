#ifndef UST_H
#define UST_H

#include <QFile>
#include <QTextDecoder>
#include <QString>
#include <QMessageBox>
#include <QDebug>

#include <vector>
#include <string>

#include "variables.h"
#include "trackSettings.h"

using namespace trackSettings;
using namespace std;

class Ust: public QObject
{
    Q_OBJECT
public:
    Ust();
    Ust(vector<string> &files, QTextDecoder *decoder = nullptr);
    Ust(const string &file, QTextDecoder *decoder = nullptr);
    ~Ust();

    void load(vector<string> &files, QTextDecoder *decoder = nullptr);
    void load(const string &file, QTextDecoder *decoder = nullptr);

    QTextDecoder *decoder() const;
    void setDecoder(QTextDecoder *newDecoder);

    double bpm() const;
    void setBpm(double newBpm);

signals:
    // Сигнал на создание ноты в PRScene
    void createNote(int length, int delta, int duration, std::string lyric, int noteNum,
                    int intensity, int modulation, short PBType, int overlap, int preutter,
                    trackSettings::Vibrato *vibrato, vector<float> *envelope, vector<int> *PBS,
                    vector<int> *PBW, vector<int> *PBY, vector<int> *PBM,
                    int noteTempo, int XPos);
    // Сигнал  на создание партии
    void createPart(std::string partName, std::string voiceDir,
                    std::string cacheDir, std::string flags);
    void resizePart(int XPos);

private:
    // Функция полоучения заголовка файла
    void parseHeader(QFile *file);
    // Фнкция получения нот из файла
    void parseNote(QFile *file);
    void parseEnvelope(string &data, vector<float> &envelope);
    void parseVibrato(string &data, Vibrato &tmpVibrato);

    QTextDecoder *_decoder = nullptr;
    double _bpm = 0.;
};

#endif // UST_H
