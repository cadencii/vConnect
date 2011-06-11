/*======================================================*/
/*                                                      */
/*    vsqEventList.h                                    */
/*                        (c) HAL 2009-                 */
/*                                                      */
/*======================================================*/
#ifndef __vsqEventList_h__
#define __vsqEventList_h__

#include "vsqBase.h"
#include "vsqEventEx.h"

class vsqEventList : public vsqBase {
public:
    ~vsqEventList();

    void    setParameter( string_t left, string_t right );

    vector<vsqEventEx*> eventList;
};

#endif
