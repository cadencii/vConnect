/*
 * UtauDB.cpp
 * Copyright © 2009-2012 HAL, 2012-2013 kbinani
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
#include "UtauDB.h"
#include "../TextInputStream.h"
#include "./Oto.h"
#include "./PrefixMap.h"

namespace vconnect
{
    struct UtauDB::Impl
    {
        Impl(std::string const& path_oto_ini, std::string const& codepage)
        {
            string const path = Path::normalize(path_oto_ini);
            root_ = make_shared<Oto>(path, codepage);
            string const directory = Path::getDirectoryName(path);
            string const prefixmap = Path::combine(directory, "prefix.map");
            prefixmap_ = make_shared<PrefixMap>(prefixmap, codepage);
            mDBPath = Path::combine(Path::getDirectoryName(path), "");
        }

        ~Impl()
        {}

        string doGetOtoIniPath() const
        {
            return mDBPath;
        }

        size_t doSize() const
        {
            return root_->count();
        }

        int doGetParamsByLyric(UtauParameter &parameters, string const& search, int note_number)
        {
            auto mapped_lyric = prefixmap_->getMappedLyric(search, note_number);
            UtauParameter * found = findParam(mapped_lyric);
            if (found) {
                parameters = *found;
                return 1;
            } else {
                UtauParameter * retry_found = findParam(search);
                if (retry_found) {
                    parameters = *retry_found;
                    return 1;
                } else {
                    return 0;
                }
            }
        }

        int doGetParamsByIndex(UtauParameter & parameters, int const index)
        {
            UtauParameter * found = (*root_)[index];
            if (found) {
                parameters = *found;
                return 1;
            } else {
                return 0;
            }
        }

    private:
        UtauParameter * findParam(string const& lyric)
        {
            return root_->find(lyric);
        }

        /// <summary>
        /// oto.iniファイルのパス．
        /// </summary>
        string mDBPath;
        std::shared_ptr<Oto> root_;
        std::shared_ptr<PrefixMap> prefixmap_;
    };

    UtauDB::~UtauDB()
    {}

    UtauDB::UtauDB(string const& path_oto_ini, string const& codepage)
        : impl_(std::make_shared<Impl>(path_oto_ini, codepage))
    {}

    int UtauDB::getParams(UtauParameter & parameters, string const& search, int note_number)
    {
        return impl_->doGetParamsByLyric(parameters, search, note_number);
    }

    string UtauDB::getOtoIniPath()
    {
        return impl_->doGetOtoIniPath();
    }

    int UtauDB::size()
    {
        return impl_->doSize();
    }

    int UtauDB::getParams(UtauParameter &parameters, int index)
    {
        return impl_->doGetParamsByIndex(parameters, index);
    }
}
