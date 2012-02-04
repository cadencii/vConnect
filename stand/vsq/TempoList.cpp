/*
 * TempoList.cpp
 * Copyright © 2010-2012 HAL
 * Copyright © 2012 kbinani
 *
 * This file is part of vConnect-STAND.
 *
 * vConnect-STAND is free software; you can redistribute it and/or
 * modify it under the terms of the GPL License.
 *
 * vConnect-STAND is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "TempoList.h"
#include "Sequence.h"

namespace vconnect
{
    const double TempoList::DEFAULT_TEMPO = 120.0;

    double TempoList::tickToSecond( long tick )
    {
        if( false == this->isUpdated ){
            this->updateTempoInfo();
        }
        double secPerClock;
        double coeff = 60.0 / Sequence::getTickPerBeat();
        map<long, TempoBP>::reverse_iterator i = this->points.rbegin();
        for( ; i != this->points.rend(); i++ ){
            if( i->first < tick ){
                double init = i->second.time;
                long delta = tick - i->first;
                secPerClock = coeff / i->second.tempo;
                return init + delta * secPerClock;
            }
        }

        secPerClock = coeff / TempoList::DEFAULT_TEMPO;
        return tick * secPerClock;
    }

    void TempoList::updateTempoInfo()
    {
        TempoBP first;
        first.tempo = TempoList::DEFAULT_TEMPO;
        first.time = 0.0;
        if( this->points.size() == 0 ){
            this->points.insert( make_pair( 0L,  first ) );
        }else{
            if( this->points.begin()->first != 0 ){
                this->points.insert( make_pair( 0L, first ) );
            }
        }
        double coeff = 60.0 / Sequence::getTickPerBeat();
        double lastTime;
        long lastClock;
        double lastTempo;
        map<long, TempoBP>::iterator i = this->points.begin();
        for( ; i != this->points.end(); i++ ){
            if( i == this->points.begin() ){
                i->second.time = 0.0;
                lastClock = 0;
                lastTempo = i->second.tempo;
                lastTime = i->second.time;
            }else{
                i->second.time = lastTime + (i->first - lastClock) * coeff / lastTempo;
                lastClock = i->first;
                lastTempo = i->second.tempo;
                lastTime = i->second.time;
            }
        }

        this->isUpdated = true;
    }

    void TempoList::push( long tick, double tempo )
    {
        this->isUpdated = false;
        TempoBP point;
        point.tempo = tempo;
        this->points.insert( make_pair( tick, point ) );
    }
}
