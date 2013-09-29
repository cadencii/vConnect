/*
 * UtauDB.cpp
 * Copyright © 2009-2012 HAL, 2012 kbinani
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

namespace vconnect
{
    struct UtauDB::Impl
    {
        Impl(std::string const& path_oto_ini, std::string const& codepage)
        {
            int result = 2;

            int index;
            string path = Path::normalize(path_oto_ini);

            TextInputStream stream(path, codepage);

            if (false == stream.ready()) {
                return;
            }

            mDBPath = Path::combine(Path::getDirectoryName(path), "");

            while (stream.ready()) {
                string line = stream.readLine();
                if (line.length() == 0){
                    continue;
                }
                UtauParameter * current = new UtauParameter(line);
                mSettingMap.insert(make_pair(current->lyric, current));
                if (current->lyric.compare(current->fileName) != 0) {
                    mSettingMap.insert(make_pair(current->fileName, current));
                }
                mSettingList.push_back(current);
            }
            if (result != 3) {
                result = 1;
            }
            stream.close();
        }

        ~Impl()
        {
            for (auto i = mSettingList.begin(); i != mSettingList.end(); ++i) {
                if ((*i)) {
                    delete (*i);
                }
            }
        }

        string doGetOtoIniPath() const
        {
            return mDBPath;
        }

        size_t doSize() const
        {
            return mSettingList.size();
        }

        int doGetParamsByLyric(UtauParameter &parameters, string const& search)
        {
            int result = 0;
            auto i = mSettingMap.find(search);
            if (i != mSettingMap.end()) {
                if (i->second) {
                    parameters.fileName         = i->second->fileName;
                    parameters.lyric            = i->second->lyric;
                    parameters.msFixedLength    = i->second->msFixedLength;
                    parameters.msLeftBlank      = i->second->msLeftBlank;
                    parameters.msPreUtterance   = i->second->msPreUtterance;
                    parameters.msRightBlank     = i->second->msRightBlank;
                    parameters.msVoiceOverlap   = i->second->msVoiceOverlap;
                    parameters.isWave           = i->second->isWave;
                    result = 1;
                }
            }
            return result;
        }

        int doGetParamsByIndex(UtauParameter & parameters, int const index)
        {
            int ret = 0;
            auto it = mSettingList.begin();
            for (int i = 0; i < doSize() && it != mSettingList.end(); i++, it++) {
                if (index == i) {
                    ret = 1;
                    parameters.fileName         = (*it)->fileName;
                    parameters.lyric            = (*it)->lyric;
                    parameters.msFixedLength    = (*it)->msFixedLength;
                    parameters.msLeftBlank      = (*it)->msLeftBlank;
                    parameters.msPreUtterance   = (*it)->msPreUtterance;
                    parameters.msRightBlank     = (*it)->msRightBlank;
                    parameters.msVoiceOverlap   = (*it)->msVoiceOverlap;
                    parameters.isWave           = (*it)->isWave;
                    break;
                }
            }
            if (it != mSettingList.end()) {
                parameters = *(*it);
            }
            return ret;
        }

    private:
        /// <summary>
        /// oto.iniファイルのパス．
        /// </summary>
        string mDBPath;
        Map<string, UtauParameter *> mSettingMap;
        list<UtauParameter *> mSettingList;
    };

    UtauDB::~UtauDB()
    {}

    UtauDB::UtauDB( string path_oto_ini, string codepage )
        : impl_(std::make_shared<Impl>(path_oto_ini, codepage))
    {}

    int UtauDB::getParams( UtauParameter &parameters, string search )
    {
        return impl_->doGetParamsByLyric(parameters, search);
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
