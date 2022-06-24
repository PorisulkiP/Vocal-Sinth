#include "TempoList.h"

double TempoList::tickToSecond( long tick )
{
    if(!isUpdated()){ updateTempoInfo(); }
    double secPerClock;
    double coeff = 60.0 / Project::getTickPerBeat();
    for(const auto& var: points){
        if(var.first < tick){
            double init = var.second.time;
            long delta = tick - var.first;
            secPerClock = coeff / var.second.tempo;
            return init + delta * secPerClock;
        }
    }

    secPerClock = coeff / Project::bpm();
    return tick * secPerClock;
}

bool TempoList::isUpdated() const
{
    return _isUpdated;
}

void TempoList::setIsUpdated(bool newIsUpdated)
{
    _isUpdated = newIsUpdated;
}

void TempoList::updateTempoInfo()
{
    TempoBP first;
    first.tempo = Project::bpm();
    first.time = 0.0;
    if( points.empty()){
        points.insert( make_pair( 0L,  first ) );
    }else{
        if( points.begin()->first != 0 ){
            points.insert( make_pair( 0L, first ) );
        }
    }
    double coeff = 60.0 / Project::getTickPerBeat();
    double lastTime;
    long lastClock;
    double lastTempo;
    for(auto& var: points){
        if(var.first == points.begin()->first){
            var.second.time = 0.0;
            lastClock = 0;
            lastTempo = var.second.tempo;
            lastTime = var.second.time;
        }
        else{
            var.second.time = lastTime + (var.first - lastClock) * coeff / lastTempo;
            lastClock = var.first;
            lastTempo = var.second.tempo;
            lastTime = var.second.time;
        }
    }
    setIsUpdated(true);
}

void TempoList::push( long tick, double tempo )
{
    setIsUpdated(false);
    TempoBP point;
    point.tempo = tempo;
    points.insert( make_pair( tick, point ) );
}
