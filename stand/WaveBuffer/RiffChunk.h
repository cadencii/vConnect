#ifndef __RIFFCHUNK_h__
#define __RIFFCHUNK_h__

namespace vconnect
{
    class RiffChunk
    {
    public:
        RiffChunk(const char *h = 0, const char *p = 0, int length = 0);
        void set(const char *h, const char *p, int length);

        bool is(const char *s) const;

        const char *data() const
        {
            return _p;
        }

        const char *header() const
        {
            return _header;
        }

        int length() const
        {
            return _length;
        }

    private:
        char _header[5];
        const char *_p;
        int _length;
    };
}

#endif
