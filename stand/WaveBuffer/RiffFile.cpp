#include "RiffFile.h"

#include <stdio.h>
#include <stdint.h>

using namespace vconnect;

RiffFile::RiffFile(const char *path)
{
    data = NULL;
    read(path);
}

RiffFile::~RiffFile(void)
{
    delete[] data;
}

bool RiffFile::read(const char *path)
{
    // ぬるぽがっ
    if(!path)
    {
        return false;
    }

    // open file.
    FILE *fp = fopen(path, "rb");
    if(!fp)
    {
        return false;
    }
    int32_t size = 0;

    // is RIFF format ?
    fread(header, 4, sizeof(char), fp);
    header[4] = 0;
    if(strcmp(header, "RIFF"))
    {
        fclose(fp);
        return false;
    }

    // get total size of the file.
    fread(&size, 1, sizeof(int32_t), fp);
    if(size < 0)
    {
        fclose(fp);
        return false;
    }
    data = new char[size];

    // read all data.
    if(size != fread(data, sizeof(char), size, fp))
    {
        fclose(fp);
        return false;
    }
    fclose(fp);

    // set chunks
    int i;
    for(i = 0; i < 4; i++)
    {
        header[i] = data[i];
    }

    while(size - i >= 8)
    {
        // ToDo:: set chunk
        char *p = &(data[i]);
        int32_t l;
        memcpy(&l, &(data[i + 4]), sizeof(int32_t));
        if(i + l + 8 > size)
        {
            return false;
        }
        RiffChunk c(p, p + 8, l);
        chunks.push_back(c);
        i += l + 8;
    }

    return true;
}

bool RiffFile::is(const char *s)
{
    return (strcmp(s, header) == 0);
}

const RiffChunk *RiffFile::chunk(const char *s)
{
    RiffChunk *c = NULL;
    for(unsigned int i = 0; i < chunks.size(); i++)
    {
        if(chunks[i].is(s))
        {
            c = &chunks[i];
            break;
        }
    }
    return c;
}
