#include <fmindex-collection/fmindex-collection.h>
#include <fmindex-collection/search/all.h>
#include <fmindex-collection/search_scheme/expand.h>
#include <fmindex-collection/search_scheme/generator/all.h>

#include <cereal/archives/binary.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <unordered_set>
#include <fmt/ranges.h>
#include <iostream>
#include <ranges>

using namespace fmindex_collection; // für die ganzen fmindex methode, fmindex_collection:: nicht notwendig 

constexpr size_t Sigma = 6; // sigma steht für alphabetgröße

template <size_t Sigma>
using String = string::InterleavedBitvector16<Sigma>; // häh was ist das? 

template <typename Index> // zur fütterung mit daten? die ersten beiden templates muss ich nicht verstehen
void saveIndex(Index const& _index, std::filesystem::path _fileName) {
    auto ofs     = std::ofstream(_fileName, std::ios::binary);
    auto archive = cereal::BinaryOutputArchive{ofs};
    archive(_index);
}

template <typename Index>
auto loadIndex(std::filesystem::path _fileName) {
    auto ifs     = std::ifstream(_fileName, std::ios::binary);
    auto archive = cereal::BinaryInputArchive{ifs};
    auto index = Index{};
    archive(index);
    return index;
}

std::vector<uint8_t> reverse_complement_generator(std::vector<uint8_t> const& ref){
    std::vector<uint8_t> reverse_complement = {}; 
    reverse_complement.reserve(size(ref)); 
    for (const uint8_t& i: ref | std::views::reverse)
    { 
        switch(i) {
            case 1:
            reverse_complement.push_back(4);
            break;

            case 2:
            reverse_complement.push_back(3);
            break;

            case 3:
            reverse_complement.push_back(2);
            break;

            case 4:
            reverse_complement.push_back(1);
            break;

            default:
            fmt::print("count error \n");
        }
    }  
    return reverse_complement;
}

int main(int argc, char const* const* argv) {
    (void)argc; // (void) sind nur für die strikten regeln des compilers
    (void)argv;

        auto chromosomes = std::vector<std::vector<uint8_t>>{{4, 4, 4, 4, 1, 2, 2, 2, 1, 2, 3, 4, 4, 4, 4}}; // unser  Text/Referenz - Vektor mit {T, T, T, T, A, C, C, C, A, C, G, T, T, T, T} 
        auto chromosomes_with_complement = chromosomes; 
        // hier wird einfach nur kopiert (optimierbar)
        // endziel: aauuüüää (mit äs und üs als reverse strings)   

        for (size_t i = 0; i < size(chromosomes); i++){ // iterator durch chromosomes
            std::vector<uint8_t> temp = reverse_complement_generator(chromosomes[i]);
            chromosomes[i].push_back(5); //{ a, b} -> { a, b, 5}
            chromosomes[i].insert(chromosomes[i].end(), temp.begin(), temp.end());  // { a, b, 5} -> { a, b, 5, b, a}
 
        }

    {
        std::cout << "\nBiFMIndex:\n";
        auto index = BiFMIndex<String<Sigma>>{chromosomes, /*samplingRate*/16, /*threadNbr*/1};
        auto queries = std::vector<std::vector<uint8_t>>{{3, 4, 3}, {2, 1, 2}}; // unser Pattern/Read - Vektor {G, T, G} {C, A, C}

        search_backtracking::search(index, queries, 0, [&](size_t queryId, auto cursor, size_t errors) {
            (void)errors;
            fmt::print("found something {} {}\n", queryId, cursor.count()); // cursor.count() == range, range begriff wird verwechselt, print fmt kombiniert printf und stdcout
            for (auto i : cursor) {  
                auto [chr, pos] = index.locate(i); // cursor stuff
                fmt::print("chr/pos: {}/{}\n", chr, pos);
            }
        });
    }
    return 0;
}
