#pragma once

#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include "hash.h"

namespace fs = boost::filesystem;

using filenames_t = std::vector<std::string>;
using path_t = fs::path;
using list_path_t = std::list<path_t>;
using mask_t = std::string;

enum ScanLevel 
{
    Current,
    All
};

using level_t = ScanLevel;

namespace def 
{
    const filenames_t Included = { "./" };
    const filenames_t Excluded = {};
    const level_t Level = Current;
    const mask_t Mask = ".*";
    const size_t Minsize = 1;
    const size_t Blocksize = 1;
    const hash::alg_t Alg = hash::AlgMd5;
}

class DuplicateFinder
{
public:

    /**
     * @brief ��� �������� DuplicateFinder ����������� �������� ������ ����������:
     * 1. ������������ ������ ������ ��� ��������� (createFilelist()).
     * �������� createFilelist():<br>
     * 1) �������������� included, excluded � ������ boost::filesystem::path (path_t).<br>
     * 2) ������ �� ���� included. � ����������� �� level - ����������� ������, ���� �������������.<br>
     * 3) ������ ������� ��������� �� ���������:
     *    - ��������� �������� ������;
     *    - ������ ����� �� ����� minsize;
     *    - ��� ����� ���������� � ������ mask.
     * � ����� ����������� ������ ������, ��������� ��� �������, ������� ����� �������� ��� ������ ������ filelist() ��� ������� � ����� ��� ������ printFilelist().<br>
     * 2. ������������ ����� ������-���������� (findDuplicates())
     * �������� (findDuplicates()):<br>
     * 1) ���� ���������� ������ <= 1, ��������� ����� �� �������, ��� ��� ���������� �� � ���. ����������� ���������� �������: ���� ����� �������� ������ � ������ �������������.<br>
     * 2) ����������� ������������� ������, ��� ������ �������� ��� ������������ �� ����� �����, � ��������� - ������ ������, ��� ������� ������ ���-�������� ���������.<br>
     * 3) ����������� ������ ������, ������ ������� ���������. ���� ����� ������ ��������� ����� 1, �� ��� ������, ��� ��� ���������� ����� ��� ��� ���������, � �� ����� ���������� � ������ � �������� � �������� ���������.<br>
     * 4) ��� ���� ������� ������ �� �������������� �������, �������� ������ 1 �������� ������� ����������.<br>
     * � ����� ����������� ������ ����� ������ ����������, ������� ����� �������� ��� ������ ������ getDuplicatesGroups() ��� ������� � ����� ��� ������ printDuplicates().<br>
     * @param included - ������ ���������� ��� ������������
     * @param excluded - ������ ���������� ��� ���������� �� ������������
     * @param mask - ����� ������������
     * @param level - ������� ������������
     * @param minsize - ����������� ������ ������
     * @param blocksize - ������ �����, �� �������� ������������ ������ ������� �����
     * @param hashalg - �������� ����������� ��������� �� ����� �����: 0 - MD5, 1 - CRC32
     */
    DuplicateFinder
    (
        const filenames_t& included,
        const filenames_t& excluded,
        const mask_t& mask,
        const level_t& level,
        const size_t& minsize,
        const size_t& blocksize,
        const hash::alg_t& hashalg
    );

    void printFilelist(std::ostream& os = std::cout);
    void printDuplicates(std::ostream& os = std::cout);

    list_path_t filelist() const;

    std::list<list_path_t> getDuplicatesGroups() const;

private:

    list_path_t m_filelist;
    std::list<list_path_t> m_DuplicatesGroups;
    size_t m_blocksize;
    hash::alg_t m_hashalg;

    /**
     * @brief ������������ ������ ������ ��� ���������
     * @param included - ������ ����������� ���������� ��� ������������
     * @param excluded - ������ ����������� ���������� ��� ������������
     * @param mask - ����� ������������
     * @param level - ������� ������������
     * @param minsize - ����������� ������ ������
     */
    void createFilelist(const filenames_t& included, const filenames_t& excluded, const mask_t& mask, const level_t& level, const size_t& minsize);

    bool contains(const list_path_t& pathContainer, const path_t& pathEntry);

    bool filtered(const path_t& pathEntry, const mask_t& mask, const size_t& minsize);

    /**
     * @brief ������������ ����� ������ ����������.
     * @param filelist - ������ ������, ���������� ���������
     * @param blockNumber - ����� �����, ������� ���������� ��������� �� ������� ����� �� ������
     */
    void findDuplicates(const list_path_t& filelist, size_t blockNumber);

    bool canCreateHash(const path_t& filename, size_t blockNumber);

    hash::Hash hashOfFileBlock(const path_t& filename, size_t blockNumber);

    static void printPathlist(const list_path_t& pathlist, std::ostream& os = std::cout);

};
