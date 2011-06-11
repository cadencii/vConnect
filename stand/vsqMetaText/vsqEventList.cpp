/*======================================================*/
/*                                                      */
/*    vsqEventList.cpp                                  */
/*                        (c) HAL 2009-                 */
/*                                                      */
/*======================================================*/
#include "vsqEventList.h"

vsqEventList::~vsqEventList()
{
    for( unsigned long i = 0; i < eventList.size(); i++ )
        if( eventList[i] )
            delete eventList[i];
}

void vsqEventList::setParameter( string_t left, string_t right ){
    /* If Back is EOS, Front is End Tick */
    string s;
    if( right.compare( _T("EOS") ) == 0 ){
        mb_conv( left, s );
        endTick = atol( s.c_str() );
    }else{
        vsqEventEx *target = new vsqEventEx;
        right = _T("[") + right + _T("]");
        mb_conv( left, s );
        target->tick = atoi( s.c_str() );
        eventList.push_back( target );
        objectMap.insert( make_pair( right, (vsqBase*)target ) );
    }
}

