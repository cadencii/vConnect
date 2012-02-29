#include "RiffChunk.h"

#include <string.h>

using namespace vconnect;

RiffChunk::RiffChunk(const char *h, const char *p, int length)
{
    _header[0] = _header[4] = '\0';
    _p = NULL;
    set(h, p, length);
}

void RiffChunk::set(const char *h, const char *p, int length)
{
    if(h)
    {
        for(int i = 0; i < 4; i++)
        {
            _header[i] = h[i];
        }
        _header[4] = '\0';
    }
    _p = p;
    _length =length;
}


bool RiffChunk::is(const char *s) const
{
    bool ret = true;
    for(int i = 0; i < 4; i++)
    {
        ret &= (s[i] == _header[i]);
    }
    return ret;
}
