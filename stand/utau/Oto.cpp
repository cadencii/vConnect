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
        explicit Impl(std::string const& oto_ini_file, std::string const& db_root_directory, std::string const& encoding)
        {
            string normalized_oto_ini_path = Path::normalize(oto_ini_file);
            string normalized_db_root_directory = Path::normalize(db_root_directory);
            oto_ini_path_ = normalized_oto_ini_path;

            TextInputStream stream(normalized_oto_ini_path, encoding);

            if (!stream.ready()) {
                return;
            }

            string directory = Path::getDirectoryName(normalized_oto_ini_path);
            string directory_prefix =
                directory.find((normalized_db_root_directory + Path::getDirectorySeparator()).c_str()) == 0
                ? directory.substr(normalized_db_root_directory.length() + 1)
                : "";

            while (stream.ready()) {
                std::string line = stream.readLine();
                if (line.empty()) {
                    continue;
                }
                shared_ptr<UtauParameter> parameter = std::make_shared<UtauParameter>(line);
                if (!directory_prefix.empty()) {
                    parameter->fileName = Path::combine(directory_prefix, parameter->fileName);
                }
                parameters_.push_back(move(parameter));
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

        std::string doGetOtoIniPath() const
        {
            return oto_ini_path_;
        }

    private:
        std::vector<std::shared_ptr<UtauParameter>> parameters_;
        std::string oto_ini_path_;
    };

    Oto::Oto(std::string const& oto_ini_file, string const& db_root_directory, std::string const& encoding)
        : impl_(std::make_shared<Impl>(oto_ini_file, db_root_directory, encoding))
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

    string Oto::getOtoIniPath() const
    {
        return impl_->doGetOtoIniPath();
    }
}

