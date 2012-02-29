#ifndef __RIFFFILE_H__
#define __RIFFFILE_H__

#include <vector>
#include "RiffChunk.h"

namespace vconnect
{

    class RiffFile
    {
    public:
        RiffFile(const char *path = 0);
        ~RiffFile();

        bool read(const char *path);
        bool is(const char *s);
        const RiffChunk *chunk(const char *s);

    private:
        char *data;
        char header[5];
        std::vector<RiffChunk> chunks;
    };

}

#endif
