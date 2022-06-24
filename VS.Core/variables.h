#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <thread>
#include <mutex>

/** Файл с глобальными переменными, которые либо не меняются, либо изменяют весь вид программы
 * масштаб сцен, кол-во нот, расстояние межну номами и т.д */

// time - милисекунда на которй мы сейчас находимся
// minTime - первая миллисикунда
// maxTime - песледняя миллисекунда. В идеале сделать таймлайн бесконечным,
extern int gTime, gMinTime, gMaxTime;
extern int gPRXOffset; // Растояние между цифрами на таймлайне
extern double gPxInMs;

// Сеттеры
extern void saveSetStrQuantize(int new_quantize);
extern void saveSetPartStrQuantize(int new_quantize);
extern void saveSetTempo(int new_tempo);
extern void saveSetMainVBFolder(const char* new_path);

extern int  saveGetStrQuantize();
extern int  saveGetIntQuantize();
extern int  saveGetPartStrQuantize();
extern int  saveGetPartIntQuantize();
extern int  saveGetTempo();
extern std::string saveGetMainVBFolder();

extern std::string gPathToMainVB;

#endif
