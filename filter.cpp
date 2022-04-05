#include "filter.h"
#include <cassert>

DuplicateFinder::DuplicateFinder(
    const filenames_t& included,
    const filenames_t& excluded,
    const mask_t& mask,
    const level_t& level,
    const size_t& minsize,
    const size_t& blocksize,
    const hash::alg_t& hashalg
)
    : m_blocksize(blocksize)
    , m_hashalg(hashalg)
{
    createFilelist(included, excluded, mask, level, minsize);
    findDuplicates(m_filelist, 0);
}

void DuplicateFinder::printFilelist(std::ostream& os)
{
    printPathlist(m_filelist, os);
}

void DuplicateFinder::printDuplicates(std::ostream& os)
{
    for (const auto& group : m_DuplicatesGroups) {
        printPathlist(group, os);
    }
}

list_path_t DuplicateFinder::filelist() const
{
    return m_filelist;
}

std::list<list_path_t> DuplicateFinder::getDuplicatesGroups() const
{
    return m_DuplicatesGroups;
}

void DuplicateFinder::createFilelist(const filenames_t& included, const filenames_t& excluded, const mask_t& mask, const level_t& level, const size_t& minsize)
{
    list_path_t includedPaths;
    list_path_t excludedPaths;
    m_filelist.clear();

    for (const auto& filename : included) {
        includedPaths.push_back(fs::canonical(path_t(filename)));
    }

    for (const auto& filename : excluded) {
        excludedPaths.push_back(fs::canonical(path_t(filename)));
    }

    for (auto& dir : includedPaths) {

        // watch if dir is in excluded list
        if (contains(excludedPaths, dir)) {
            continue;
        }

        if (level == All) {
            for (const auto& entry : fs::recursive_directory_iterator(dir)) {

                // watch if entry is in excluded list
                if (contains(excludedPaths, dir)) {
                    continue;
                }

                if (filtered(entry, mask, minsize)) {
                    m_filelist.push_back(entry);
                }
            }
        }
        else {
            for (const auto& entry : fs::directory_iterator(dir)) {

                if (filtered(entry, mask, minsize)) {
                    m_filelist.push_back(entry);
                }

            }
        }
    }
}

bool DuplicateFinder::contains(const list_path_t& pathContainer, const path_t& pathEntry)
{
    return std::find(pathContainer.begin(), pathContainer.end(), pathEntry)
        != pathContainer.end();
}

bool DuplicateFinder::filtered(const path_t& pathEntry, const mask_t& mask, const size_t& minsize)
{
    // check if path is a file
    if (!fs::is_regular_file(pathEntry))
        return false;

    // check if filesize >= minsize
    if (fs::file_size(pathEntry) < minsize)
        return false;

    // check if mask matched
    boost::smatch what;
    if (!boost::regex_match(pathEntry.filename().string(), what,
        boost::regex(mask, boost::regex::icase)))
        return false;

    return true;
}

void DuplicateFinder::findDuplicates(
    const list_path_t& filelist,
    size_t blockNumber
)
{
    if (filelist.size() <= 1)
        return;

    std::map<hash::Hash, list_path_t> map;
    list_path_t duplicates;
    for (const auto& filename : filelist) 
    {
        if (canCreateHash(filename, blockNumber)) 
            map[hashOfFileBlock(filename, blockNumber)].push_back(filename);
        else 
            duplicates.push_back(filename);

    }

    if (duplicates.size() > 1)
        m_DuplicatesGroups.push_back(duplicates);

    for (auto key : map) 
    {
        if (key.second.size() > 1) 
            findDuplicates(key.second, blockNumber + 1);
    }
}

bool DuplicateFinder::canCreateHash(const path_t& filename, size_t blockNumber)
{
    return blockNumber * m_blocksize < fs::file_size(filename);
}

hash::Hash DuplicateFinder::hashOfFileBlock(const path_t& filename, size_t blockNumber)
{
    assert(blockNumber * m_blocksize <= fs::file_size(filename));

    std::ifstream file(filename.string(), std::ios::binary | std::ios::ate);
    file.seekg(blockNumber * m_blocksize, std::ios::beg);
    std::vector<char> buffer(m_blocksize);
    auto size = m_blocksize;

    // если не кратно дописать нули
    if ((blockNumber + 1) * m_blocksize > fs::file_size(filename)) 
    {
        size = fs::file_size(filename) - blockNumber * m_blocksize;
        std::memset(buffer.data(), 0, m_blocksize);
    }

    // чтение данных в буфер
    if (!file.read(buffer.data(), size)) 
    {
        throw std::runtime_error("Error reading file " + filename.string());
    }

    return hash::Hash(buffer.data(), m_blocksize, m_hashalg);
}

void DuplicateFinder::printPathlist(const list_path_t& pathlist, std::ostream& os)
{
    for (const auto& path : pathlist) 
        os << fs::canonical(path).string()<< std::endl;
    os << std::endl;
}