#include "ust.h"

Ust::Ust(){}

Ust::~Ust(){}

Ust::Ust(vector<string> &files, QTextDecoder *decoder)
{
    if (decoder) { setDecoder(decoder); }
    load(files);
}

Ust::Ust(const string &file, QTextDecoder *decoder)
{
    if (decoder) { setDecoder(decoder); }
    load(file);
}

void Ust::load(vector<string> &files, QTextDecoder *decoder)
{
    if (decoder) {setDecoder(decoder);}
    for (auto &&var : files) { load(var); }
}

void Ust::load(const string &filePath, QTextDecoder *decoder)
{
    if (decoder) {setDecoder(decoder);}
    QFile file(filePath.data());
    file.open(QIODevice::ReadOnly);

    parseHeader(&file); // Считывается заголовок файла
    try { } catch (...) { qDebug() << "Не удалось прочитать заголовок файла"; }

    parseNote(&file); // Считываеются ноты из файла
    try { } catch (...) { qDebug() << "Не удалось считать все ноты файла"; }
    file.close();
}

void Ust::parseHeader(QFile *file){
    string line, data;
    string partName, voiceDir, cacheDir, flags;
    double bpm = 0.;
    size_t found;

    while (strcmp(line.data(), "[#0000]") != 0) {
        // Считывается строка из файла
        line = decoder()->toUnicode(file->readLine()).remove(-2, 2).toStdString();
        // Считываются данные после знака равенства
        found = line.find("=");
        if (found == string::npos) {continue;}
        data = line.substr(found+1);
        // из строки удаляются все символы после знака =
        line.erase(found);

        // В зависимости от строки устанавливаются значения
        if(line == "UstName" ||
           line == "ProjectName")       { partName = data;          }
        else if(line == "VoiceDir")     { voiceDir = data;          }
        else if(line == "Tempo")        { bpm = atof(data.c_str()); }
        else if(line == "CachDir")      { cacheDir = data;          }
        else if(line == "Flags")        { flags = data;             }
    }
    setBpm(bpm);
    // Отправляется сигнал на создание партии.
    emit createPart(partName.data(),
                    voiceDir.data(),
                    cacheDir.data(),
                    flags.data());
}

void Ust::parseNote(QFile *file){
    string line, data;
    int length = 0;
    int delta = 0;
    int duration = 0;
    string lyric = "a";
    int noteNum = 60; // Высота ноты
    int intensity = 100;
    int modulation = 0;
    short PBType = 0;
    int overlap = 0;
    int preutter = 0;
    int noteTempo = 100;
    Vibrato tmpVibrato;
    vector<float> envelope;
    vector<int> PBW;
    vector<int> PBS;
    vector<int> PBY;
    vector<int> PBM;

    std::size_t found;
    int XPos = 0;

    while (!file->atEnd()) {
        // Считывается строка из файла
        line = decoder()->toUnicode(file->readLine()).remove(-2, 2).toStdString();
        found = line.find("=");
        // Если прочтённая строка будет значением [#xxxx], то отправляется сигнал на создание ноты
        if ((!line.find("[#"))) {
            if (strcmp(lyric.data(), "R")){
                emit createNote(length, delta, duration, lyric, noteNum, intensity, modulation,
                                PBType, overlap, preutter, &tmpVibrato, &envelope, &PBS, &PBW,
                                &PBY, &PBM, noteTempo, XPos);
            }
            XPos += length;
            continue;
        }

        if (found == std::string::npos) { continue; }

        // Считываются данные после знака равенства
        data = line.substr(found+1);
        // из строки удаляются все символы после знака =
        line.erase(found);

        // В зависимости от строки устанавливаются значения
        if(line == "Length")                           { length = atoi(data.c_str())/18;   }
        else if(!strcmp(line.data(),"Delta"))          { delta = atoi(data.c_str());       }
        else if(!strcmp(line.data(),"Duration"))       { duration = atoi(data.c_str());    }
        else if(!strcmp(line.data(),"Lyric"))          { lyric = data;                     }
        else if(!strcmp(line.data(),"NoteNum"))        { noteNum = atoi(data.c_str());     }
        else if(!strcmp(line.data(),"Intensity"))      { intensity = atoi(data.c_str());   }
        else if(!strcmp(line.data(),"Modulation"))     { modulation = atoi(data.c_str());  }
        else if(!strcmp(line.data(),"PBType"))         { PBType = atoi(data.c_str());      }
        else if(!strcmp(line.data(),"VoiceOverlap"))   { overlap = atoi(data.c_str());     }
        else if(!strcmp(line.data(),"PreUtterance"))   { preutter = atoi(data.c_str());    }
        else if(!strcmp(line.data(),"Envelope"))       { parseEnvelope(data, envelope);    }
        else if(!strcmp(line.data(),"VBR"))            { parseVibrato(data, tmpVibrato);   }
        else if(!strcmp(line.data(),"PBS"))            { PBS.push_back(atoi(data.c_str()));}
        else if(!strcmp(line.data(),"PBW"))            { PBW.push_back(atoi(data.c_str()));}
        else if(!strcmp(line.data(),"PBY"))            { PBY.push_back(atoi(data.c_str()));}
        else if(!strcmp(line.data(),"PBM"))            { PBM.push_back(atoi(data.c_str()));}
        else if(!strcmp(line.data(),"Tempo"))          { noteTempo = atoi(data.c_str());   }
        else { continue; }

        // UST Version < 2.0
        // | length       | length       |
        // | note1        | R            |

        // UST Version = 2.0
        // | length1      | length2      |
        // | dur1  |      | dur2         |
        // | note1 | R    | note2        |
        // | delta2
    }
    emit resizePart(XPos);
}

void Ust::parseEnvelope(string &data, vector<float> &envelope) {
    // p1,p2,p3,v1,v2,v3,v4,%,p4,p5,v5
    // (0,5, 35,0,100,100,0,%,0 ,0,100)
    try {
        envelope = vector<float>(data.begin(), data.end());
//        if (envelope.size() < 7) {
//            return;
//        }
//        float p1 = envelope.at(0), p2 = envelope.at(1), p3 = envelope.at(2), v1 = envelope.at(3), v2 = envelope.at(4), v3 = envelope.at(5), v4 = envelope.at(6);
//        if (envelope.size() == 11) {
//            float p4 = envelope.at(8), p5 = envelope.at(9), v5 = envelope.at(10);
//        }
    } catch (...) {
        qDebug() << "Invalid Envelope:" << data.data();
    }
}

void Ust::parseVibrato(string &data, Vibrato &tmpVibrato) {
    try {
        vector<int> args (data.begin(), data.end());
        if (args.size() >= 1) {
            tmpVibrato.setLength(args[0]);
        }
        if (args.size() >= 2) {
            tmpVibrato.setPeriod(args[1]);
        }
        if (args.size() >= 3) {
            tmpVibrato.setDepth(args[2]);
        }
        if (args.size() >= 4) {
            tmpVibrato.setIn(args[3]);
        }
        if (args.size() >= 5) {
            tmpVibrato.setOut(args[4]);
        }
        if (args.size() >= 6) {
            tmpVibrato.setShift(args[5]);
        }
        if (args.size() >= 7) {
            tmpVibrato.setDrift(args[6]);
        }
    } catch (...) {
        qDebug() << "Invalid VBR:" << data.data();
    }
}

double Ust::bpm() const
{
    return _bpm;
}

void Ust::setBpm(double newBpm)
{
    _bpm = newBpm;
}

QTextDecoder *Ust::decoder() const
{
    return _decoder;
}

void Ust::setDecoder(QTextDecoder *newDecoder)
{
    _decoder = newDecoder;
}
