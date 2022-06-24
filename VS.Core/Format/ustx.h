#ifndef USTX_H
#define USTX_H

#include <vector>

#include "project.h"

using namespace std;

namespace core { class Ustx; }

class Ustx
{
public:
    Ustx();

    string UstxVersion = "0, 5";

    const string DYN = "dyn";
    const string PITD = "pitd";
    const string CLR = "clr";
    const string ENG = "eng";
    const string VEL = "vel";
    const string VOL = "vol";
    const string ATK = "atk";
    const string DEC = "dec";
    const string GEN = "gen";
    const string GENC = "genc";
    const string BRE = "bre";
    const string BREC = "brec";
    const string LPF = "lpf";
    const string MOD = "mod";
    const string ALT = "alt";
    const string SHFT = "shft";
    const string SHFC = "shfc";
    const string TENC = "tenc";
    const string VOIC = "voic";

    vector<string> required = { DYN, PITD, CLR, ENG, VEL, VOL, ATK, DEC};

    static Project Create();

};

#endif // USTX_H
