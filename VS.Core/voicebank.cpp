#include "voicebank.h"

Voicebank::Voicebank() {}

string Voicebank::notesFiender(const string &syllable){
    foreach (auto &var, _mainOtoSet->otos()) {
        string tmpAlias = var->getAlias();
        if (!strcmp(var->getAlias().c_str(), syllable.c_str())){
            return var->getWav().data();
        }
    }
    return "";
}


void Voicebank::engToJP(const string &syllable){}

void Voicebank::reload(){
    name = "";
    image = "";
    portrait = "";
    portraitOpacity = 0;
    author = "";
    voice = "";
    web = "";
    version = "";
    otherInfo = "";
    txtEncoding = "";
    otoSets.clear();
    subbanks.clear();
    id = "0";
}

float Voicebank::getPortraitOpacity() const
{
    return portraitOpacity;
}

void Voicebank::setPortraitOpacity(float newPortraitOpacity)
{
    portraitOpacity = newPortraitOpacity;
}

const string &Voicebank::getAuthor() const
{
    return author;
}

void Voicebank::setAuthor(const string &newAuthor)
{
    author = newAuthor;
}

const string &Voicebank::getVoice() const
{
    return voice;
}

void Voicebank::setVoice(const string &newVoice)
{
    voice = newVoice;
}

const string &Voicebank::getWeb() const
{
    return web;
}

void Voicebank::setWeb(const string &newWeb)
{
    web = newWeb;
}

const string &Voicebank::getBasePath() const
{
    return basePath;
}

void Voicebank::setBasePath(const string &newBasePath)
{
    basePath = saveGetMainVBFolder();
    basePath.append(newBasePath);
    if (basePath.find("%voice%") != string::npos ||
        basePath.find("%VOICE%") != string::npos){
        basePath.erase(basePath.find_first_of("%"), 7);
    }
    load();
}

const string &Voicebank::getCachDir() const
{
    return cachDir;
}

void Voicebank::setCachDir(const string &newCachDir)
{
    cachDir = newCachDir;
}

const string &Voicebank::getFile() const
{
    return file;
}

void Voicebank::setFile(const string &newFile)
{
    file = newFile;
}

const string &Voicebank::getName() const
{
    return name;
}

void Voicebank::setName(const string &newName)
{
    name = newName;
}

const string &Voicebank::getImage() const
{
    return image;
}

void Voicebank::setImage(const string &newImage)
{
    image = newImage;
}

const string &Voicebank::getFlags() const
{
    return flags;
}

void Voicebank::setFlags(const string &newFlags)
{
    flags = newFlags;
}

const string &Voicebank::getVersion() const
{
    return version;
}

void Voicebank::setVersion(const string &newVersion)
{
    version = newVersion;
}

const string &Voicebank::getOtherInfo() const
{
    return otherInfo;
}

void Voicebank::setOtherInfo(const string &newOtherInfo)
{
    otherInfo = newOtherInfo;
}

const string &Voicebank::getTxtEncoding() const
{
    return txtEncoding;
}

void Voicebank::setTxtEncoding(const string &newTxtEncoding)
{
    txtEncoding = newTxtEncoding;
}

const string &Voicebank::getId() const
{
    return id;
}

void Voicebank::setId(const string &newId)
{
    id = newId;
}

OtoSet *Voicebank::mainOtoSet() const
{
    return _mainOtoSet;
}

void Voicebank::setMainOtoSet(OtoSet *newMainOtoSet)
{
    _mainOtoSet = newMainOtoSet;
}

bool Voicebank::findedPrefixMap() const
{
    return _findedPrefixMap;
}

void Voicebank::setFindedPrefixMap(bool newFindedPrefixMap)
{
    _findedPrefixMap = newFindedPrefixMap;
}

const string &Voicebank::getPortrait() const
{
    return portrait;
}

void Voicebank::setPortrait(const string &newPortrait)
{
    portrait = newPortrait;
}

QAbstractItemModel *Voicebank::modelFromOto(const OtoSet& otoSet)
{
    // ?????????????????? ???????????? ?????? ????????
    QStringList words;
    // ???????????? ???? ???????????? ???????????????????? ?? ????????????
    for(const auto& oto : otoSet.otos()){
        words << oto->getAlias().data();
    }
    // ???????????????????????? ???????????? ???? ?????????????? ???? ??????????
    return new QStringListModel(words, _completer);
}

void Voicebank::load(){
    // ???????????????????? ?????????? oto ?? prefix.map ???????????? //
    // ?????????????????? ?????????? ?? ???????????????????? ????????
    QDirIterator it(basePath.data(), QDir::Dirs);
//    qDebug() << basePath.data();

    while(it.hasNext()){
        QString tmpPath = it.next();
        tmpPath.remove(tmpPath.lastIndexOf("/"));

        QFile file(tmpPath);
        if (file.exists()){ // ???????? ???????????? oto ????????
            if (!findedPrefixMap()){ // ???????? ???? ???????????? prefix.map
                prefixMap = new PrefixMap(basePath + "\\prefix.map", "UTF-8");
                // ???????? prefixMap->config ????????, ???????????? prefix.map ????????
                if (prefixMap->configs().empty()) {
                    delete prefixMap;
                    prefixMap = nullptr;
                }
                else { setFindedPrefixMap(true); }
            }

            if(!prefixMap){
                prefixMap = new PrefixMap();
                prefixMap->setHaveNoteName(0);
            }
            OtoSet *otoSet = Oto::parseOto(tmpPath.toStdString(), prefixMap->haveNoteName());
            otoSets.push_back(otoSet);
        }
    }
    if (!otoSets.empty()){
        for (const auto& set : otoSets) {
            if (!set->file().empty() && set->otos().size() > 10){
                _mainOtoSet = set;
                qDebug() << *set;
                setCompleter(set->file());
                break;
            }
        }
    }
}
