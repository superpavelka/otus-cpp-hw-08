#pragma once

#include <cstring>
#include <vector>
#include <boost/uuid/detail/md5.hpp>
#include <boost/crc.hpp>
#include <boost/algorithm/hex.hpp>

namespace hash {

    using boost::uuids::detail::md5;

    enum HashAlgorithm
    {
        AlgMd5,
        AlgCrc32
    };

    using alg_t = HashAlgorithm;
    using byte_t = uint8_t;

    class Hash
    {
    public:

        Hash() = default;

        ~Hash() = default;

        Hash(const void* buffer, std::size_t bytes, alg_t alg = AlgMd5);

        Hash(const Hash& other);

        std::string toString() const;

        friend inline bool operator<(const Hash& first, const Hash& second);

        friend inline bool operator==(const Hash& first, const Hash& second);

        friend inline bool operator!=(const Hash& first, const Hash& second);

    private:

        std::vector<byte_t> m_data;

        static void calc_md5(const void* ibuf, std::size_t bytes, void* obuf);
        static void calc_crc32(const void* ibuf, std::size_t bytes, void* obuf);

    };

    inline bool operator<(const Hash& first, const Hash& second)
    {
        if (first.m_data.size() != second.m_data.size()) {
            return false;
        }

        decltype(first.m_data.size()) i = 0;
        while (i < first.m_data.size()) 
        {
            if (first.m_data.data()[i] == second.m_data.data()[i]) 
                i++;
            else 
                return first.m_data.data()[i] < second.m_data.data()[i];
        }
        return false;
    }

    inline bool operator==(const Hash& first, const Hash& second)
    {
        if (first.m_data.size() != second.m_data.size()) 
            return false;
        return std::memcmp(first.m_data.data(), second.m_data.data(), first.m_data.size()) == 0;
    }

    inline bool operator!=(const Hash& first, const Hash& second)
    {
        return !(first == second);
    }

}