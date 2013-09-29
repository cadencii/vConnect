/*
 * Oto.h
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
#include <vector>

namespace vconnect
{
    class UtauParameter;

    class Oto
    {
        typedef std::vector<UtauParameter *> container_type;

    public:
        explicit Oto(std::string const& oto_ini_file, std::string const& encoding);

        ~Oto()
        {}

        UtauParameter * find(std::string const& lyric);

    private:
        Oto(Oto const& lhs);
        Oto & operator = (Oto const& lhs);

        struct Impl;
        std::shared_ptr<Impl> impl_;
    };
}
