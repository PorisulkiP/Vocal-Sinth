#include "PrefixMap.h"

PrefixMap::PrefixMap(string const& prefixmap_file_path, string const& encoding)
{
    QFile *file = new QFile(prefixmap_file_path.data());
    if (file->open(QIODevice::ReadOnly)) {
        // Считывание файла
        QTextCodec *defaultTextCodec = QTextCodec::codecForName(encoding.data());
        QTextDecoder *decoder = new QTextDecoder(defaultTextCodec);
        while (!file->atEnd()) {
            string line = decoder->toUnicode(file->readLine()).remove(-2, 2).toStdString();

            size_t firtT, secondT;

            firtT = line.find("\t");
            if (firtT == std::string::npos) { continue; }
            string strNote;
            if (firtT <  line.size()){
                strNote = QString(line.substr(0, firtT).data()).remove("\t").toStdString();
            }
            else{
                strNote = QString(line.data()).remove("\t").toStdString();
            }

            int note_number = MusicMath::noteNumFromName(strNote);
            if (note_number == -1) { continue; }

            Config *config = new Config();
            secondT = line.find_last_of("\t");
            if ((secondT + 1) <  line.size()){
                config->prefix_ = QString(line.substr(firtT, secondT + 1).data()).remove("\t").toStdString();
            }
            else{
                config->prefix_ = QString(line.substr(firtT).data()).remove("\t").toStdString();
            }
            auto str = QString(line.substr(secondT).data()).remove("\t").toStdString();
            // Если суфикс не повторяет префикс
            if (config->prefix_ != str) {
                config->suffix_ = str;
                setHaveNoteName(2);
            }
            else {
                config->suffix_ = string();
                setHaveNoteName(1);
            }
            if (haveNoteName() == 0) {
                if (config->prefix_.empty()) {  setHaveNoteName(1); }
                else                         {  setHaveNoteName(2); }
            }
//            configs_[note_number] = *config;
        }
    }
    // Prefix Map
    // ---------------------------------------------
    // | note name      | prefix       | suffix    |
    // | A#7            | B4           |           |
    // | B7             |              | B4        |
    // ---------------------------------------------
}

PrefixMap::operator QString() const  {
    auto var = configs().begin();
    return QString("config size:" + QString::number(configs_.size()) +
                   " note num:" + QString::number(var->first) +
                   " prefix:" + QString::fromStdString(var->second.prefix_) +
                   " suffix:" + QString::fromStdString(var->second.suffix_).remove("\t") + ")");
}

const map<int, Config> &PrefixMap::configs() const
{
    return configs_;
}

uint8_t PrefixMap::haveNoteName() const
{
    return _haveNoteName;
}

void PrefixMap::setHaveNoteName(uint8_t newHaveNoteName)
{
    _haveNoteName = newHaveNoteName;
}

PrefixMap::~PrefixMap(){}

int PrefixMap::getNoteNumber(string const& s) const
{
    if (s.length() > 0) {
        char const key_char = s[0];
        int key = 0;
        switch (key_char) {
        case 'C':
            key = 0;
            break;
        case 'D':
            key = 2;
            break;
        case 'E':
            key = 4;
            break;
        case 'F':
            key = 5;
            break;
        case 'G':
            key = 7;
            break;
        case 'A':
            key = 9;
            break;
        case 'H':
        case 'B':
            key = 11;
            break;
        default: return 0;
        }
        int const flat_accidentals =
            count_if(begin(s), end(s), [](char const ch) {
                return ch == 'b';
            });
        int const sharp_accidentals =
            count_if(begin(s), end(s), [](char const ch) {
                return ch == '#';
            });
        string scale_string;
        for_each(begin(s), end(s), [&scale_string](char ch) {
            if (isdigit(ch)) {
                scale_string += ch;
            }
        });
        int const scale = atoi(scale_string.c_str());
        return (scale + 2) * 12 + key + sharp_accidentals - flat_accidentals;
    } else {
        return 0;
    }
}

string PrefixMap::getMappedLyric(string const& lyric, int const note_number) const
{
    auto it = configs_.find(note_number);
    if (it != configs_.end()) {
        auto config = configs_.at(note_number);
        return config.prefix_ + lyric + config.suffix_;
    }
    else { return lyric; }
}
