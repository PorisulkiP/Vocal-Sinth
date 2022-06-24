#ifndef TEMPOLIST_H
#define TEMPOLIST_H

#include <string>
#include <map>

#include "../project.h"

using namespace std;

class TempoBP{
public:
    double tempo; // BPM
    double time; // Время в секундах
};

class TempoList
{
public:
    TempoList(){ setIsUpdated(false); }

    // Добавьте набор значений темпа и времени в список темпа
    void push( long tick, double tempo );

    double tickToSecond( long tick );

    bool isUpdated() const;
    void setIsUpdated(bool newIsUpdated);

private:

    // Ассоциативный набор изменений темпа
    // со временем в тактах, в качестве ключа
    map<long, TempoBP> points;

    // Было ли обновлено время изменения темпа в содержимом поля точек
    bool _isUpdated;

    // Обновление временной части в секундах
    void updateTempoInfo();
};

#endif // TEMPOLIST_H
