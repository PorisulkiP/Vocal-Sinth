#ifndef MUSICMATH_H
#define MUSICMATH_H

#include <cmath>
#include <string>
#include <cstring>
#include <vector>
#include <string_view>

#include "trackSettings.h"

using namespace trackSettings;
using namespace std;

class MusicMath
{
public:
    MusicMath();

    static string noteNameFromNum(int noteNum);
    static int noteNumFromName(const string& noteName);
    static bool isNoteName(const string& noteName);

    static double tickToMillisecond(double tick, double BPM, int beatUnit, int resolution) noexcept;
    static int millisecondToTick(double ms, double BPM, int beatUnit, int resolution) noexcept;

    static double sinEasingInOut(double x0, double x1, double y0, double y1, double x);
    static double sinEasingInOutX(double x0, double x1, double y0, double y1, double y);
    static double sinEasingIn(double x0, double x1, double y0, double y1, double x);
    static double sinEasingInX(double x0, double x1, double y0, double y1, double y);
    static double sinEasingOut(double x0, double x1, double y0, double y1, double x);
    static double sinEasingOutX(double x0, double x1, double y0, double y1, double y);

    static double linear(double x0, double x1, double y0, double y1, double x);

    static double decibelToLinear(double db);
    static double linearToDecibel(double v);

    static double toneToFreq(int tone);
    static double toneToFreq(const string &tone);
    static double toneToFreq(int tone, double mainFreq);

    static int freqToTone(double freq);

    static double InterpolateShape(double x0, double x1, double y0, double y1, double x, PitchPointShape shape);

    static constexpr double minFreq = 16.35;
    static constexpr double maxFreq = 3951.07;
    static constexpr int octCount = 7; // Количество октав
    static constexpr int noteOneOct = 12; // Количество нот в одной октаве (7+5)
    static constexpr int fullnotesCount = octCount * noteOneOct; // Всего нот (7+5)*7 = 84

    // string_view быстрее, чем string
    static constexpr string_view notesName[] = { "B7", "A#7", "A7", "G#7", "G7", "F#7", "F7", "E7", "D#7", "D7", "C#7", "C7",
                                                 "B6", "A#6", "A6", "G#6", "G6", "F#6", "F6", "E6", "D#6", "D6", "C#6", "C6",
                                                 "B5", "A#5", "A5", "G#5", "G5", "F#5", "F5", "E5", "D#5", "D5", "C#5", "C5",
                                                 "B4", "A#4", "A4", "G#4", "G4", "F#4", "F4", "E4", "D#4", "D4", "C#4", "C4",
                                                 "B3", "A#3", "A3", "G#3", "G3", "F#3", "F3", "E3", "D#3", "D3", "C#3", "C3",
                                                 "B2", "A#2", "A2", "G#2", "G2", "F#2", "F2", "E2", "D#2", "D2", "C#2", "C2",
                                                 "B1", "A#1", "A1", "G#1", "G1", "F#1", "F1", "E1", "D#1", "D1", "C#1", "C1"};
    template<class T>
    static void deleteArray(T& x) {
        if(x){
            delete[] (x); x = NULL;
        }
    }
private:
    constexpr static double DoublePi = 2 * M_PI;
    constexpr static double DoubleDevidedPi = M_PI / 2;
    constexpr static double LOG_2_DB = 8.68588963806503655302257837833210;
    constexpr static double DB_2_LOG = 0.11512925464970228420089957273422;
};


#endif // MUSICMATH_H
