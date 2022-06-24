#include "trackSettings.h"

using namespace trackSettings;

template<class Project, class Track>
double Pitch::sample(Project *project, Track *note, double tick) {
//    for (int i = 0; i < static_cast<int>(note->pitch.data->size()) - 1; i++) {
//        PitchPoint p1 = note->pitch.data[i];
//        int t1 = note->startPos.x() + project->millisecondToTick(p1.X);
//        PitchPoint p2 = note->pitch.data[i + 1];
//        int t2 = note->startPos.x() + project->millisecondToTick(p2.X);
//        if (t1 <= tick && tick <= t2) {
//            return MusicMath::InterpolateShape(
//                       t1, t2, p1.Y(), p2.Y(), tick, p1.shape()) * 10;
//        }
//    }
//    PitchPoint pFirst = note->pitch.data->at(0);
//    int tFirst = note->startPos.x() + project->millisecondToTick(pFirst.X);
//    if (tick < tFirst) {
//        return pFirst.Y() * 10;
//    }
//    PitchPoint pLast = note->pitch.data->at(note->pitch.data->size() - 1);
//    int tLast = note->startPos.x() + project->millisecondToTick(pLast.X);
//    if (tick > tLast) {
//        return pLast.Y() * 10;
//    }
    return -1;
}

