#include "Oto.h"

Oto::operator QString() const  {
    return QString("Alias:" + QString(getAlias().data()) +
                   " (path to wav: " + QString(getWav().data()) +
                   ", Offset: "  + QString::number(getOffset())  +
                   ", Consonant:"  + QString::number(getConsonant()) +
                   ", Cutoff:"  + QString::number(getCutoff()) +
                   ", Preutter:"  + QString::number(getPreutter()) +
                   ", Overlap:"  + QString::number(getOverlap()) + ")");
}

string Oto::info()
{
    return string("\"" + getAlias() +
                  "\" (path to wav: " + getWav() +
                    ", Offset: "  + to_string(getOffset())  +
                    ", Consonant:"  + to_string(getConsonant()) +
                    ", Cutoff:"  + to_string(getCutoff()) +
                    ", Preutter:"  + to_string(getPreutter()) +
                    ", Overlap:"  + to_string(getOverlap()) + ")");
}

OtoSet* Oto::parseOto(const string &fileName, uint8_t nameFlag){
    size_t found;
    auto tmp = fileName;

    // Удаление /.. из строки
    found = tmp.find_last_of(".");
    if (found != string::npos){
        tmp.erase(found - 1);
    }

    // В сете ото данных устанавливается путь до фала
    OtoSet* otoSet = new OtoSet(tmp + "\\oto.ini");
    otoSet->setHaveNoteName(nameFlag);
    // Откртытие файла
    QFile file(otoSet->file().data());

    // из него удаляется C:\\Program Files (x86)\\UTAU\\voice
    tmp.erase(0, saveGetMainVBFolder().length());

    // После удаления всего после \\, устанавливается как имя ГБ
    otoSet->setName(tmp);

    // Если не удаётся открыть файл
    if (!file.open(QFile::ReadOnly)){
        return new OtoSet(); // возвращается пустая модель
    }

    QTextDecoder *decoder = new QTextDecoder(QTextCodec::codecForName("Shift-JIS"));
    // пока файл не закончится
    while (!file.atEnd()) {
        auto oto = new Oto();
        // Считывается линия
        string line = decoder->toUnicode(file.readLine()).toStdString();
        if (line.empty()) { continue; }

        string tmp = line;
        for (auto i = 0; i < 6; ++i){
            auto found = tmp.find_last_of(",") + 1;
            if (found == string::npos){ continue; }
            switch (i) {
            case 0:
                oto->setOverlap(atoi(tmp.substr(found).c_str()));
                tmp.erase(--found);
                break;
            case 1:
                oto->setPreutter(atoi(tmp.substr(found).c_str()));
                tmp.erase(--found);
                break;
            case 2:
                oto->setCutoff(atoi(tmp.substr(found).c_str()));
                tmp.erase(--found);
                break;
            case 3:
                oto->setConsonant(atoi(tmp.substr(found).c_str()));
                tmp.erase(--found);
                break;
            case 4:
                oto->setOffset(atoi(tmp.substr(found).c_str()));
                tmp.erase(--found);
                break;
            case 5:
                // Находятся знаки после =
                found = tmp.find("=") + 1;
                if (found == string::npos){ continue; }
                if (otoSet->haveNoteName() == 1){
                    string alias = tmp.substr(found);
                    if (alias.empty()) {continue;}
                    // Удаляются два последних символа
                    oto->setAlias(alias.erase(alias.length() - 2));
                }
                else if (otoSet->haveNoteName() == 2){
                    // Удаляются два символа спереди
                    oto->setAlias(tmp.substr(found + 2).data());
                }
                else{ oto->setAlias(tmp.erase(0, found).data()); }
                break;
            default: break;
            }
        }
        auto found = line.find("=");
        if (found == string::npos){ continue; }
        oto->setWav(line.erase(found).data());

        if (oto->getAlias().empty() && !otoSet->haveNoteName()){
            auto foundPoint = line.find(".");
            auto foundSlesh = line.find_last_of("/");
            if (foundPoint == string::npos && foundSlesh == string::npos )
            { continue; }
            line.erase(foundPoint);
            if (foundSlesh != string::npos){
                line.erase(0, foundSlesh);
            }
            oto->setAlias(line.data());
        }
        otoSet->appendOtos(oto);
    }
    return otoSet;
}

OtoSet::operator QString() const
{
    return QString(("file:" + QString(file().data()) +
                    + "\n (name: " + QString(name().data())
                    + "\n haveNoteName: "  + QString::number(haveNoteName())  + "\n"
                   + *otos().at(0) + ")"));
}
uint8_t OtoSet::haveNoteName() const
{
    return _haveNoteName;
}

void OtoSet::setHaveNoteName(uint8_t newHaveNoteName)
{
    _haveNoteName = newHaveNoteName;
}
