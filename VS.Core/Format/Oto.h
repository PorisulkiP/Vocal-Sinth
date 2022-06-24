#ifndef OTO_H
#define OTO_H

#include <QFile>
#include <QDebug>
#include <QString>
#include <QTextDecoder>

#include <string>
#include <vector>

#include "variables.h"
#include "musicMath.h"

using namespace std;

class OtoSet;

class Oto
{
public:
    Oto(){}
    static OtoSet* parseOto(const string &fileName, uint8_t nameFlag = 0);

    // Для вывода в cout
    friend ostream& operator<<(ostream& stream, const Oto &o) noexcept{
        return stream << "\"" << o.getAlias() <<
               "\" (path to wav: " << o.getWav() <<
               ", Offset: "  << o.getOffset()  <<
               ", Consonant:"  << o.getConsonant() <<
               ", Cutoff:"  << o.getCutoff() <<
               ", Preutter:"  << o.getPreutter() <<
               ", Overlap:"  << o.getOverlap() << ")";
    }

    // Для вывода в qDebug()
    operator QString() const;

    string info();

    const string &getAlias() const              {  return Alias;            }
    void setAlias(const string &newAlias)       {  Alias = newAlias;        }

    const string &getWav() const                { return Wav;               }
    void setWav(const string &newWav)           { Wav = newWav;             }

    double getOffset() const                    { return offset;            }
    void setOffset(double newOffset)            { offset = newOffset;       }

    double getConsonant() const                 { return consonant;         }
    void setConsonant(double newConsonant)      { consonant = newConsonant; }

    double getCutoff() const                    { return cutoff;            }
    void setCutoff(double newCutoff)            { cutoff = newCutoff;       }

    double getPreutter() const                  { return preutter;          }
    void setPreutter(double newPreutter)        { preutter = newPreutter;   }

    double getOverlap() const                   { return overlap;           }
    void setOverlap(double newOverlap)          { overlap = newOverlap;     }

private:
    string Alias;
    string Phonetic;
    string Wav;    

    // Структура Wav:

    // Смещение(Offset) – отступ, от начала, который не войдёт в семпл.

    // Отсечка(Cutoff) – смещение, но с конца.
    //          Положительные значения — отсчет с конца семпла, отрицательные – с offset.

    // Консонант(Consonant) – Разделяет звук на две части – "гласную" и "согласную".
    //          "Гласная" будет растягиваться или зацикливаться, если увеличивать длину ноты.

    // Перекрытие(Overlap) – Именно область от offset до overlap будет смешиваться с предыдущей нотой.
    //          Соответственно чем она больше, тем мягче переход.

    // preutter – "предварительно произнесенное".
    //          Все, что до Preutterance, будет выговорено до начала ноты,
    //          все что после – соответственно, в пределах.

    // |-offset-|-consonant-(fixed)-|-stretched-|-cutoff-|
    // |        |-preutter-----|
    // |        |-overlap-|
    // Позиция ноты:
    // ... ----------предыдущая-нота-|-эта-нота-- ...
    // Фонемное перекрытие:
    // ... --предыдущая-фонема-\
    //                /-эта-фонема-------------- ...

    // Длинна левого отступа.
    double offset = 0;
    // Length of unstretched consonant in wav, AKA fixed.
    double consonant = 0;
    // Length of right cutoff, AKA end blank. If negative, length of (consonant + stretched).
    double cutoff = 0;
    // Length before note start, usually within consonant range.
    double preutter = 0;
    // Length overlap with previous note, usually within consonant range.
    double overlap = 0;
};

class OtoSet
{

public:
    OtoSet(){}
    OtoSet(const string &newFile)           { setFile(newFile);                 }
    void setFile(const string &newFile)     { _file = newFile;                  }
    void setName(const string &newName)     { _name = newName;                  }
    void appendOtos(Oto *newOto)            { _otos.emplace_back(newOto);       }
    void setErorrs(const string &newErorr)  { _errors.emplace_back(newErorr);   }

    // Для вывода в qDebug()
    operator QString() const;

    string file() const                     { return _file; }
    string name() const                     { return _name; }
    vector<Oto*> otos() const               { return _otos; }
    vector<string> erors() const            { return _errors; }

    uint8_t haveNoteName() const;
    void setHaveNoteName(uint8_t newHaveNoteName);

private:
    // 0 - нет ни суфикса, ни префикса
    // 1 - есть суфикс
    // 2 - есть префикс
    uint8_t _haveNoteName = 0;
    string _file;
    string _name;
    vector<Oto*> _otos;
    vector<string> _errors;
};

class Subbank
{
public:
    Subbank(){}
    ~Subbank(){}

    const string &getColor() const          { return color; }
    void setColor(const string &newColor)   { color = newColor; }

    const string &getPrefix() const         { return prefix; }
    void setPrefix(const string &newPrefix) { prefix = newPrefix; }

    const string &getSuffix() const         { return suffix; }
    void setSuffix(const string &newSuffix) { suffix = newSuffix; }

private:
    string color;
    string prefix;
    string suffix;
    vector<string> toneRanges;// Tone ranges. Each range specified as "C1-C4" or "C4".
};

#endif // OTO_H
