/*
 * PrefixMap.cpp
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
#include <map>
#include <algorithm>
#include <cctype>
#include "./PrefixMap.h"
#include "../TextInputStream.h"
#include "../StringUtil.h"

namespace vconnect
{
    struct PrefixMap::Impl
    {
    public:
        explicit Impl(std::string const& prefixmap_file_path, std::string const& encoding)
        {
            TextInputStream stream(prefixmap_file_path, encoding);
            while (stream.ready()) {
                string const line = stream.readLine();
                auto parameters = StringUtil::explode("\t", line, 3);
                if (parameters.size() < 3) {
                    continue;
                }
                string const note = parameters[0];
                int const note_number = getNoteNumber(note);
                Config config;
                config.prefix_ = parameters[1];
                config.suffix_ = parameters[2];
                configs_[note_number] = config;
            }
        }

        ~Impl()
        {}

        string doGetMappedLyric(string const& lyric, int const note_number)
        {
            auto it = configs_.find(note_number);
            if (it != configs_.end()) {
                auto config = configs_[note_number];
                return config.prefix_ + lyric + config.suffix_;
            } else {
                return lyric;
            }
        }

    private:
        int getNoteNumber(string const& s) const
        {
            if (s.length() > 0) {
                char const key_char = s[0];
                int key = 0;
                switch (key_char) {
                    case 'C':
                        key = 0;
                        break;
                    case 'D':
                        key = 2;
                        break;
                    case 'E':
                        key = 4;
                        break;
                    case 'F':
                        key = 5;
                        break;
                    case 'G':
                        key = 7;
                        break;
                    case 'A':
                        key = 9;
                        break;
                    case 'H':
                    case 'B':
                        key = 11;
                        break;
                    default: return 0;
                }
                int const flat_accidentals =
                    std::count_if(std::begin(s), std::end(s), [](char const ch) {
                        return ch == 'b';
                    });
                int const sharp_accidentals =
                    std::count_if(std::begin(s), std::end(s), [](char const ch) {
                        return ch == '#';
                    });
                string scale_string;
                std::copy_if(std::begin(s), std::end(s), std::begin(scale_string), [](char const ch) {
                    return std::isdigit(ch);
                });
                int const scale = std::atoi(scale_string.c_str());
                return (scale + 2) * 12 + key + sharp_accidentals - flat_accidentals;
            } else {
                return 0;
            }
        }

        struct Config
        {
            string prefix_;
            string suffix_;
        };

        std::map<int, Config> configs_;
    };

    PrefixMap::PrefixMap(std::string const& prefixmap_file_path, std::string const& encoding)
        : impl_(std::make_shared<Impl>(prefixmap_file_path, encoding))
    {}

    PrefixMap::~PrefixMap()
    {}

    std::string PrefixMap::getMappedLyric(std::string const& lyric, int const note_number) const
    {
        return impl_->doGetMappedLyric(lyric, note_number);
    }
}
