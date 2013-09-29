/*
 * Oto.cpp
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
#include <algorithm>
#include "./Oto.h"
#include "../TextInputStream.h"
#include "./UtauParameter.h"

namespace vconnect
{
    struct Oto::Impl
    {
        explicit Impl(std::string const& oto_ini_file, std::string const& encoding)
        {
            TextInputStream stream(oto_ini_file, encoding);

            if (!stream.ready()) {
                return;
            }

            while (stream.ready()) {
                std::string line = stream.readLine();
                if (line.empty()) {
                    continue;
                }
                parameters_.push_back(std::make_shared<UtauParameter>(line));
            }
        }

        ~Impl()
        {}

        UtauParameter * doFind(std::string const& lyric)
        {
            auto result =
                std::find_if(std::begin(parameters_), std::end(parameters_), [lyric](std::shared_ptr<UtauParameter> const& parameter) {
                    return (parameter->lyric == lyric) || (parameter->fileName == lyric);
                });
            if (result != std::end(parameters_)) {
                return result->get();
            } else {
                return nullptr;
            }
        }

        size_t doCount() const
        {
            return parameters_.size();
        }

        UtauParameter * doGet(size_t const index) const
        {
            return parameters_[index].get();
        }

    private:
        std::vector<std::shared_ptr<UtauParameter>> parameters_;
    };

    Oto::Oto(std::string const& oto_ini_file, std::string const& encoding)
        : impl_(std::make_shared<Impl>(oto_ini_file, encoding))
    {}

    UtauParameter * Oto::find(std::string const& lyric) { return impl_->doFind(lyric); }

    size_t Oto::count() const
    {
        return impl_->doCount();
    }

    UtauParameter * Oto::operator [] (size_t const index) const
    {
        if (index < impl_->doCount()) {
            return impl_->doGet(index);
        } else {
            return nullptr;
        }
    }
}
