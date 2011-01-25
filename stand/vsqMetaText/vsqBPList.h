/*
 *
 *    vsqBPList.h
 *                        (c) HAL 2010-
 *
 *  This files is a part of v.Connect.
 * vsqBPList class contains and reads vsqBP data.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __vsqBPLish_h__
#define __vsqBPList_h__

#include "vsqBase.h"
#include "vsqBP.h"

class vsqBPList : vsqBase{
public:
    vsqBPList(){}
    void    getList( vector<standBP>& dst );
    void    setParameter( long tick, int value );
protected:
    void    setParameter( string_t left, string_t right );
private:
    vector<vsqBP> data;
};

#endif
