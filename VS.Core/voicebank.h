#ifndef VOICEBANK_H
#define VOICEBANK_H

#include <QFile>
#include <QDebug>
#include <QCompleter>
#include <QTextDecoder>
#include <QFileSystemModel>
#include <QStringListModel>
#include <QAbstractItemModel>

#include "Format/Oto.h"
#include "PrefixMap.h"

using namespace std;

class Voicebank
{
public:
    Voicebank();

    void load();
    void reload();
    QAbstractItemModel *modelFromOto(const OtoSet& otoSet);
    string notesFiender(const string &syllable);
    void engToJP(const string &syllable);

    QCompleter *completer() const     { return _completer;    }
    void setCompleter(const string &path)
    {
        _completer = new QCompleter(modelFromOto(path));
    }

    const string &getBasePath() const;
    void setBasePath(const string &newBasePath);

    const string &getCachDir() const;
    void setCachDir(const string &newCachDir);

    const string &getFile() const;
    void setFile(const string &newFile);

    const string &getName() const;
    void setName(const string &newName);

    const string &getImage() const;
    void setImage(const string &newImage);

    const string &getFlags() const;
    void setFlags(const string &newFlags);
    
    const string &getPortrait() const;
    void setPortrait(const string &newPortrait);
    
    float getPortraitOpacity() const;
    void setPortraitOpacity(float newPortraitOpacity);
    
    const string &getAuthor() const;
    void setAuthor(const string &newAuthor);
    
    const string &getVoice() const;
    void setVoice(const string &newVoice);
    
    const string &getWeb() const;
    void setWeb(const string &newWeb);

    const string &getVersion() const;
    void setVersion(const string &newVersion);

    const string &getOtherInfo() const;
    void setOtherInfo(const string &newOtherInfo);

    const string &getTxtEncoding() const;
    void setTxtEncoding(const string &newTxtEncoding);

    const string &getId() const;
    void setId(const string &newId);

    vector<Oto*> getOto(const string &otoType = ""){
        return _mainOtoSet->otos();
    }

    OtoSet *mainOtoSet() const;
    void setMainOtoSet(OtoSet *newMainOtoSet);

    bool findedPrefixMap() const;
    void setFindedPrefixMap(bool newFindedPrefixMap);

private:
    string basePath;
    string cachDir;
    string file;
    string name;
    string image;
    string portrait;
    float  portraitOpacity = 100;
    string author;
    string voice;
    string web;
    string flags;
    string version;
    string otherInfo;
    string txtEncoding = "shift_jis";
    // 1 oto.ini = 1 OtoSet
    vector<OtoSet*> otoSets;
    vector<Subbank*> subbanks;
    string id;

    PrefixMap *prefixMap = nullptr;
    bool _findedPrefixMap = false;

    // Создаётся набор Ото файлов
    OtoSet *_mainOtoSet = new OtoSet();

    QTextDecoder *decoder = new QTextDecoder(QTextCodec::codecForName("Shift-JIS"));
    QCompleter *_completer = nullptr; // Автозаполнитель
};


#endif // VOICEBANK_H
