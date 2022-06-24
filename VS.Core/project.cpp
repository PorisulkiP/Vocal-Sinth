#include "project.h"

#define VB_FROM_SQL

bool Project::PLAY_STATE = false;
double Project::_bpm = 120;
int Project::prjStrQuantize = 4;
int Project::prjIntQuantize = 1. / prjStrQuantize * 100;

int Project::prjPartStrQuantize = 4;
int Project::prjPartIntQuantize = 1./prjPartStrQuantize*100;

std::vector<std::string> Project::VBitems;

Project::Project(const string& _prjPath) {
    // Если проект открывается, то не создаётся временный файл
    if(!_prjPath.empty()){
        open(_prjPath);
    }
    else{ // Если проект создаётся, то до сохранения создаётся его временный файл
        prjPath = "C:\\Windows\\Temp\\VStemp";
        time_t t = time(NULL);
        string a = ctime(&t);
        std::replace_if(a.begin(), a.end(),
                        [](char c)
                        {return c == L' ' || c == L':' || c == L'\n';},
                        L'_');
        prjPath.append(a);
        prjPath.pop_back();
        prjPath.append(".vsprj");

        FILE *f = fopen(prjPath.data(), "w");
        setDefaultValues();
        fclose(f);
    }
}

Project::~Project() {
    // удаление временного файла проекта
    qDebug() << "saved:" << _saved;
    if(remove(prjPath.data()) != 0 && _saved == false)
        qInfo( "Ошибка удаления файла" );
    else
        qInfo( "Файл успешно удалён" );
}

Types Project::openFileType() const
{
    return _openFileType;
}

void Project::setOpenFileType(Types newOpenFileType)
{
    _openFileType = newOpenFileType;
}

void Project::setOpened(bool newOpened)
{
    _opened = newOpened;
}

void Project::setAppending(bool newAppending)
{
    _appending = newAppending;
}

QSqlDatabase* Project::connectToDataBase()
{
#ifdef VB_FROM_SQL
    QString host = "LAPTOP-9U5B74NH\\SQLEXPRESS";
    QString database = "Голосовой синтезатор";

    db = QSqlDatabase::addDatabase("QODBC", "main_connection");
    db.setDatabaseName(QString("DRIVER={SQL Server};"
                               "Server=%1;"
                               "Database=%2;"
                               "Trusted_Connection=True;").arg(host, database));
    if (!db.open()){
        QMessageBox::critical(qobject_cast<QWidget*>(this), tr("Ошибка"),
                              tr("Ошибка подключения к базе данных: %2").arg(db.lastError().text()));
        return new QSqlDatabase();
    }

    return &db;
#endif
}

double Project::bpm()
{
    return _bpm;
}

void Project::setBpm(double newBpm)
{
    _bpm = newBpm;
    saveSetTempo(newBpm);
}

Ust *Project::ust() const
{
    return _ust;
}

void Project::setUst(Ust *newUst)
{
    _ust = newUst;
}

void Project::open(const string& filePath){
    prjPath = filePath;
    setOpened(true);
    append(prjPath);
    setOpened(false);
}

void Project::save(){
    _saved = true;
    _modified = false;
}

void Project::setDefaultValues(){
    std::ofstream out(prjPath.data());

    if (out.is_open())
    {
        out << "g_time = 0" << endl;
        out << "g_minTime = 0" << endl;
        out << "g_maxTime = 180000" << endl;
        out << "g_PR_xOffset = 50" << endl;
        out << "g_str_quantize = 12" << endl;
        out << "g_int_quantize = 8" << endl;
        out << "g_part_str_quantize = 12" << endl;
        out << "g_part_int_quantize = 8" << endl;
        out << "g_tempo = 180" << endl;
        out << "g_path_to_mainVB = C:\\Program Files (x86)\\UTAU\\voice\\雷鳴スイゲツ_RAIMEI SUIGETSU(Len)" << endl;

        // Резeрв под другие параметры
//            out << "g_time = 0" << endl;
//            out << "g_time = 0" << endl;
//            out << "g_time = 0" << endl;
//            out << "g_time = 0" << endl;
    }
    out.close();
}

void Project::append(const string& filePath){
    if (!opened()) { setAppending(true); }

    if(filePath.find(".ust") != string::npos){
        setOpenFileType(Types::UST);
        ust()->load(filePath, decoder);
        if (opened()) { saveSetTempo(ust()->bpm()); }
    }
    else if(filePath.find(".ustx") != string::npos){
        qInfo( "Выбрат проект типа USTx" );
        setOpenFileType(Types::USTx);
    }
    else if(filePath.find(".vsqx") != string::npos){
        qInfo( "Выбрат проект типа VSQX" );
        setOpenFileType(Types::VSQx);
    }
    else {

        qInfo() <<  QString::fromStdString(filePath)
                << "- данный тип файла невозможно открыть как проект";
    }
    setAppending(false);
}

