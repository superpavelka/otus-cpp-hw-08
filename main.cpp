#include <boost/program_options.hpp>
#include "filter.h"

namespace options = boost::program_options;

int main(int argc, const char* argv[])
{

    try 
    {

        filenames_t included, excluded;
        mask_t mask;
        size_t level, minsize, blocksize, hashtype;

        options::options_description desc{ "Options" };
        desc.add_options()
            ("help,h", "Suppported commands")
            ("incdir,i", options::value<filenames_t>(&included)->composing(), "directories for scan")
            ("excdir,e", options::value<filenames_t>(&excluded)->composing(), "excluded directories for scan")
            ("level,l", options::value<size_t>(&level)->default_value(def::Level), "scan level: 1 - recursive, 0 - current directory only")
            ("mask,m", options::value<mask_t>(&mask)->composing()->default_value(def::Mask), "regex: masks of files")
            ("minsize,s", options::value<size_t>(&minsize)->default_value(def::Minsize), "minimal size of file")
            ("blocksize,b", options::value<size_t>(&blocksize)->default_value(def::Blocksize), "compare block size")
            ("hashtype,t", options::value<size_t>(&hashtype)->default_value(def::Alg), "hash type: 0 - md5, 1 - crc32")
            ;

        options::variables_map vm;
        options::store(parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) 
        {
            std::cout << desc << '\n';
        }
        else 
        {

            options::notify(vm);

            if (included.empty())
                included = def::Included;

            if (blocksize < def::Blocksize)
                blocksize = def::Blocksize;

            DuplicateFinder scanner(included, excluded, mask, static_cast<level_t>(level), minsize, blocksize, static_cast<hash::alg_t>(hashtype));
            scanner.printDuplicates();

        }

    }
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
    }
}