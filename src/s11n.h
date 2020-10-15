//  Copyright (c) 2014-2020 Andrey Upadyshev <oliora@gmail.com>
//
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "ppconsul/config.h"
#include "ppconsul/error.h"
#include <rapidjson/rapidjson.h>
#include <vector>
#include <chrono>
#include <set>
#include <map>
#include <string>

namespace ppconsul { namespace s11n {

    using Json = rapidjson::Value;

    namespace detail {
        inline Json parse_json(const std::string& s)
        {
            rapidjson::Document document;
            auto parse_result = document.Parse(s);
            if (!parse_result)
            {
                throw FormatError(GetParseError_En(parse_result.Code()));
            }
            return document;
        }
    }

    inline std::string to_json(std::chrono::milliseconds ms)
    {
        return std::to_string(ms.count()) + "ms";
    }

    inline std::string to_json(std::chrono::seconds s)
    {
        return std::to_string(s.count()) + "s";
    }

    inline std::string to_json(std::chrono::minutes m)
    {
        return std::to_string(m.count()) + "m";
    }

    inline void load(const Json& src, uint16_t& dst)
    {
        dst = (src.IsUInt()) ? static_cast<uint16_t>(src.UInt()) : 0;
    }

    inline void load(const Json& src, bool& dst)
    {
        dst = (src.IsBool()) ? src.GetBool() : false; 
    }

    inline void load(const Json& src, int& dst)
    {
        dst = (src.IsInt()) ? src.GetInt() : 0;
    }

    inline void load(const Json& src, uint64_t& dst)
    {
        dst = (src.IsUInt64()) ? src.GetUInt64() : 0;
    }

    inline void load(const Json& src, std::chrono::minutes& dst)
    {
        dst = std::chrono::minutes((src.IsUInt()) ? src.GetUInt() : 0);
    }

    inline void load(const Json& src, std::string& dst)
    {
        dst = (src.IsString()) ? src.GetString() : "";
    }

    template<class T>
    void load(const Json& src, std::vector<T>& dst)
    {
        dst.clear();

        if (!src.IsArray())
        {
            return;
        }

        const auto& arr = src.GetArray();

        dst.reserve(arr.Size());

        for (const auto& i : arr)
        {
            T t;
            load(i, t);
            dst.push_back(std::move(t));
        }
    }

    template<class T>
    void load(const Json& src, std::set<T>& dst)
    {
        dst.clear();

        if (!src.IsArray())
        {
            return;
        }

        const auto& arr = src.GetArray();

        for (const auto& i : arr)
        {
            T t;
            load(i, t);
            dst.insert(std::move(t));
        }
    }

    template<class T>
    void load(const Json& src, std::map<std::string, T>& dst)
    {
        dst.clear();

        if (!src.IsObject())
        {
            return;
        }

        const auto& obj= src.GetObject();

        for (const auto& i : obj)
        {
            T t;
            load(i.value, t);
            dst.emplace(i.name, std::move(t));
        }
    }

    template<class T>
    void load(const Json& src, T& dst, const char *name)
    {
        if (src.HasMemeber(name))
        {
            load(src[name], dst);
        }
        else
        {
            load(Json{}, dst);
        }

    }

    template<class T>
    T parseJson(const std::string& jsonStr)
    {
        using namespace s11n;
        
        auto obj = detail::parse_json(jsonStr);
        T t;
        load(obj, t);
        return t;
    }
}}
