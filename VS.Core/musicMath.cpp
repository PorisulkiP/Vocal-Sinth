#include "musicMath.h"

MusicMath::MusicMath(){}

double MusicMath::toneToFreq(const string &tone)
{
    // Ищется схожая с tone строка
    for(size_t i = 0; i < fullnotesCount-1; ++i){
        if (notesName[i] == tone){ return toneToFreq(i+1); }
    }
    return -1;
}

//function noteFromPitch( frequency ) {
//    var noteNum = 12 * (Math.log( frequency / 440 )/Math.log(2) );
//    return Math.round( noteNum ) + 69;
//}

//function frequencyFromNoteNumber( note ) {
//    return 440 * Math.pow(2,(note-69)/12);
//}

//function centsOffFromPitch( frequency, note ) {
//    return Math.floor( 1200 * Math.log( frequency / frequencyFromNoteNumber( note ))/Math.log(2) );
//}

double MusicMath::toneToFreq(int tone) {
    return 440.0 * pow(2, (tone - 69)/12); // 440 это A4
}

double MusicMath::toneToFreq(int tone, double mainFreq) {
    return mainFreq * pow(2, (tone - 69)/12); // 440 это A4
}

int MusicMath::freqToTone(double freq)
{
    auto noteNum = 12 * (log(freq / 440 )/log(2) );
    return noteNum;

    if (freq < minFreq || freq > maxFreq){ return -1;}
    int lo = 0;
    for (size_t var = 0; var < fullnotesCount - 1; ++var) {
        auto tmp = toneToFreq(var);
        if (tmp <= freq && tmp >= lo) { lo = var; }
    }
    return lo;
}

double MusicMath::InterpolateShape(double x0, double x1, double y0, double y1, double x, PitchPointShape shape) {
    switch (shape) {
        case io: return sinEasingInOut(x0, x1, y0, y1, x);
        case i: return sinEasingIn(x0, x1, y0, y1, x);
        case o: return sinEasingOut(x0, x1, y0, y1, x);
        default: return linear(x0, x1, y0, y1, x);
    }
}

string MusicMath::noteNameFromNum(int noteNum){
    for(int i = 0; i < fullnotesCount; ++i){
        if (i == (noteNum - 1)){ return notesName[i].data(); }
    }
    return "";
}

int MusicMath::noteNumFromName(const string &noteName){
    for(int i = 0; i < fullnotesCount; ++i){
        if (notesName[i] == noteName) { return i; }
    }
    return -1;
}

bool MusicMath::isNoteName(const string &noteName)
{
    for (const auto& var : notesName) {
        if (noteName.find(var.data()) != std::string::npos){
            return true;
        }
    }
    return false;
}



double MusicMath::tickToMillisecond(double tick, double BPM, int beatUnit, int resolution) noexcept {
    return tick * 60000.0 / BPM * beatUnit / 4 / resolution;
}

int MusicMath::millisecondToTick(double ms, double BPM, int beatUnit, int resolution) noexcept {
    return int(ceil(ms / 60000.0 * BPM / beatUnit * 4 * resolution));
}

double MusicMath::sinEasingInOut(double x0, double x1, double y0, double y1, double x) {
    return y0 + (y1 - y0) * (1 - cos((x - x0) / (x1 - x0) * M_PI)) / 2;
}

double MusicMath::sinEasingInOutX(double x0, double x1, double y0, double y1, double y) {
    return acos(1 - (y - y0) * 2 / (y1 - y0)) / M_PI * (x1 - x0) + x0;
}

double MusicMath::sinEasingIn(double x0, double x1, double y0, double y1, double x) {
    return y0 + (y1 - y0) * (1 - cos((x - x0) / (x1 - x0) * DoubleDevidedPi));
}

double MusicMath::sinEasingInX(double x0, double x1, double y0, double y1, double y) {
    return acos(1 - (y - y0) / (y1 - y0)) / DoublePi * (x1 - x0) + x0;
}

double MusicMath::sinEasingOut(double x0, double x1, double y0, double y1, double x) {
    return y0 + (y1 - y0) * sin((x - x0) / (x1 - x0) * DoubleDevidedPi);
}

double MusicMath::sinEasingOutX(double x0, double x1, double y0, double y1, double y) {
    return asin((y - y0) / (y1 - y0)) / DoublePi * (x1 - x0) + x0;
}

double MusicMath::linear(double x0, double x1, double y0, double y1, double x) {
    return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
}

double MusicMath::decibelToLinear(double db) {
    return exp(db * DB_2_LOG); //pow(10, db / 20)
}

double MusicMath::linearToDecibel(double v) {
    return log(v) * LOG_2_DB;
}
