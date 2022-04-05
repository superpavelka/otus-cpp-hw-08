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
     * @brief При создании DuplicateFinder выполняется алгоритм поиска дубликатов:
     * 1. Формирование списка файлов для сравнения (createFilelist()).
     * Алгоритм createFilelist():<br>
     * 1) Преобразование included, excluded в формат boost::filesystem::path (path_t).<br>
     * 2) Проход по всем included. В зависимости от level - рекурсивный проход, либо нерекурсивный.<br>
     * 3) Фильтр каждого вхождения по критериям:
     *    - вхождение является файлом;
     *    - размер файла не менее minsize;
     *    - имя файла сочетсатся с маской mask.
     * В итоге формируется список файлов, прошедших все фильтры, который можно получить при помощи метода filelist() или вывести в поток при помощи printFilelist().<br>
     * 2. Формирование групп файлов-дубликатов (findDuplicates())
     * Алгоритм (findDuplicates()):<br>
     * 1) Если количество файлов <= 1, досточный выход из функции, так как снавнивать не с чем. Выполняется требование задания: блок файла читается только в случае необходимости.<br>
     * 2) Формируется ассоциативный массив, где ключом является хэш прочитанного из файла блока, а значением - список файлов, для которых данное хэш-значение одинаково.<br>
     * 3) Формируется список файлов, чтение которых завершено. Если таких файлов оказалось более 1, то это значит, что все предыдущие блоки для них одинаковы, и их можно объединить в группу и записать в итоговый результат.<br>
     * 4) Для всех списков файлов из ассоциативного массива, размером больше 1 вызываем функцию рекурсивно.<br>
     * В итоге формируется список групп файлов дубликатов, который можно получить при помощи метода getDuplicatesGroups() или вывести в поток при помощи printDuplicates().<br>
     * @param included - список директорий для сканирования
     * @param excluded - список директорий для исключения из сканирования
     * @param mask - маска сканирования
     * @param level - уровень сканирования
     * @param minsize - минимальный размер файлов
     * @param blocksize - размер блока, по которому производится четние каждого файла
     * @param hashalg - алгоритм хэширования читаемого из файла блока: 0 - MD5, 1 - CRC32
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
     * @brief Формирование списка файлов для сравнения
     * @param included - список добавленных директорий для сканирования
     * @param excluded - список исключенных директорий для сканирования
     * @param mask - маска сканирования
     * @param level - уровень сканирования
     * @param minsize - минимальный размер файлов
     */
    void createFilelist(const filenames_t& included, const filenames_t& excluded, const mask_t& mask, const level_t& level, const size_t& minsize);

    bool contains(const list_path_t& pathContainer, const path_t& pathEntry);

    bool filtered(const path_t& pathEntry, const mask_t& mask, const size_t& minsize);

    /**
     * @brief Формирование групп файлов дубликатов.
     * @param filelist - список файлов, подлежащих сравнению
     * @param blockNumber - номер блока, который необходимо прочитать из каждого файла из списка
     */
    void findDuplicates(const list_path_t& filelist, size_t blockNumber);

    bool canCreateHash(const path_t& filename, size_t blockNumber);

    hash::Hash hashOfFileBlock(const path_t& filename, size_t blockNumber);

    static void printPathlist(const list_path_t& pathlist, std::ostream& os = std::cout);

};
