/*
 * PrefixMap.h
 * Copyright © 2013 kbinani
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
#pragma once

#include <memory>
#include <string>

namespace vconnect
{
    class PrefixMap
    {
    public:
        PrefixMap(std::string const& prefixmap_file_path, std::string const& encoding);
        virtual ~PrefixMap();
        std::string getMappedLyric(std::string const& lyric, int const note_number) const;

    private:
        PrefixMap(PrefixMap const& lhs);
        PrefixMap & operator = (PrefixMap const& lhs);

        struct Impl;
        std::shared_ptr<Impl> impl_;
    };
}
