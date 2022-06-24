#ifndef PREFIXMAP_H
#define PREFIXMAP_H

#include <QFile>
#include <QTextDecoder>
#include <QString>
#include <QDebug>

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>

#include "variables.h"
#include "musicMath.h"

using namespace std;

struct Config
{
    string prefix_;
    string suffix_;
};

class PrefixMap
{
public:
    PrefixMap(){}
    PrefixMap(string const& prefixmap_file_path, string const& encoding);
    virtual ~PrefixMap();

    // Для вывода в qDebug()
    operator QString() const;

    int getNoteNumber(string const& s) const;
    string getMappedLyric(string const& lyric, int const note_number) const;

    const map<int, Config> &configs() const;
    uint8_t haveNoteName() const;
    void setHaveNoteName(uint8_t newHaveNoteName);

private:
    // 0 - нет ни суфикса, ни префикса
    // 1 - есть суфикс
    // 2 - есть префикс
    uint8_t _haveNoteName = 0;
    PrefixMap(PrefixMap const& lhs);
    PrefixMap &operator= (PrefixMap const& lhs);
    map<int, Config> configs_;
};

#endif // PREFIXMAP_H
