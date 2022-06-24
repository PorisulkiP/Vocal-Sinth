#include "variables.h"

// текущее время, мс
int gTime = 0;
// минимальное время для ВСЕГО проекта, мс
int gMinTime = 0;
// конец ВСЕГО проекта. 3 минуты = 180 000мс по умолчанию.
int gMaxTime = 360000;

// Растояние между секндами на линии времени
// Подбирал на глаз, но вроде ок.
int gPRXOffset = 100;

double gPxInMs = 0;

/* Кванта́йз (англ. quantize/quantization) — перемещение нот к ближайшим ритмическим долям в секвенсорах.
 * Автоматически корректируйте настройку положения ввода детали на значение (кроме ВЫКЛ(NONE)),
 * выбранное в раскрывающемся списке отображения.
 * Начало введенной и перемещенной детали становится позицией строки установочного значения.
 * 1 / 2,4,6,8,12,16,32,64; None */
int gStrQuantize = 8;
int gIntQuantize = (int)((1./gStrQuantize*100));

int gPartStrQuantize = 8;
int gPartIntQuantize = (int)((1./gPartStrQuantize*100));

int gTempo = 120;

std::string gPathToMainVB("C:\\Program Files (x86)\\UTAU\\voice\\AERIS VC-CV ENG Kire\\E4_Natural");
std::string gPathToMainVBFolder("C:\\Program Files (x86)\\UTAU\\voice\\");

// мьютекс, защищающий g_i
std::mutex g_mutex;

// потокобезопасная установка нового квантования
void saveSetStrQuantize(int new_quantize)
{
    // после выхода из функции мьютекс автоматически разблокируется
    std::lock_guard<std::mutex> lock(g_mutex);
    gStrQuantize = new_quantize;
    gIntQuantize = (int)(1./gStrQuantize*100);
}

// потокобезопасный получатель квантования
int saveGetStrQuantize()
{
    // после выхода из функции мьютекс автоматически разблокируется
    std::lock_guard<std::mutex> lock(g_mutex);
    return gStrQuantize;
}

// потокобезопасный получатель итогового квантования
int saveGetIntQuantize()
{
    // после выхода из функции мьютекс автоматически разблокируется
    std::lock_guard<std::mutex> lock(g_mutex);
    return gIntQuantize;
}

// потокобезопасная установка нового темпа
void saveSetPartStrQuantize(int new_quantize)
{
    // после выхода из функции мьютекс автоматически разблокируется
    std::lock_guard<std::mutex> lock(g_mutex);
    gPartStrQuantize = new_quantize;
    gPartIntQuantize = (int)(1./gStrQuantize*100);
    g_mutex.unlock();
}

// потокобезопасная установка нового пути до папки с голосовыми банками
void saveSetMainVBFolder(const char * new_path)
{
    // после выхода из функции мьютекс автоматически разблокируется
    g_mutex.lock();
    gPathToMainVBFolder = new_path;
    g_mutex.unlock();
}

// потокобезопасный получатель пути до папки с голосовыми банками
std::string saveGetMainVBFolder(){
    // после выхода из функции мьютекс автоматически разблокируется
    std::lock_guard<std::mutex> lock(g_mutex);
    return gPathToMainVBFolder;
}

// потокобезопасный получатель квантования в сцене с партиями
int saveGetPartStrQuantize()
{
    // после выхода из функции мьютекс автоматически разблокируется
    std::lock_guard<std::mutex> lock(g_mutex);
    return gPartStrQuantize;
}

// потокобезопасный получатель итогового квантования в сцене с партиями
int saveGetPartIntQuantize()
{
    // после выхода из функции мьютекс автоматически разблокируется
    std::lock_guard<std::mutex> lock(g_mutex);
    return gPartIntQuantize;
}

// потокобезопасная установка нового темпа
void saveSetTempo(int new_tempo)
{
    // после выхода из функции мьютекс автоматически разблокируется
    std::lock_guard<std::mutex> lock(g_mutex);
    gTempo = new_tempo;
}

// потокобезопасный получатель темпа
int saveGetTempo()
{
    // после выхода из функции мьютекс автоматически разблокируется
    std::lock_guard<std::mutex> lock(g_mutex);
    return gTempo;
}



