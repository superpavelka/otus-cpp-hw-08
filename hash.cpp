#include "hash.h"

using namespace hash;

Hash::Hash(const void* buffer, std::size_t bytes, alg_t alg)
{
    if (alg == AlgMd5) 
    {
        m_data.resize(sizeof(md5::digest_type));
        calc_md5(buffer, bytes, m_data.data());
    }
    else 
    {
        m_data.resize(sizeof(uint32_t));
        calc_crc32(buffer, bytes, m_data.data());
    }
}

Hash::Hash(const Hash& other) : m_data(other.m_data)
{
}

std::string Hash::toString() const
{
    std::string result;
    boost::algorithm::hex(m_data.data(), m_data.data() + m_data.size(),
        std::back_inserter(result));
    return result;
}

void Hash::calc_md5(const void* ibuf, std::size_t bytes, void* obuf)
{
    md5 hash;
    hash.process_bytes(ibuf, bytes);
    md5::digest_type value;
    hash.get_digest(value);
    std::memcpy(obuf, value, sizeof(md5::digest_type));
}

void Hash::calc_crc32(const void* ibuf, std::size_t bytes, void* obuf)
{
    boost::crc_32_type crc32;
    crc32.process_bytes(ibuf, bytes);
    auto value = crc32.checksum();
    std::memcpy(obuf, &value, sizeof(value));
}
