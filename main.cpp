#include <fmindex-collection/fmindex-collection.h>
#include <fmindex-collection/search/all.h>
#include <fmindex-collection/search_scheme/expand.h>
#include <fmindex-collection/search_scheme/generator/all.h>

#include <cereal/archives/binary.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <unordered_set>

using namespace fmindex_collection; // für die ganzen fmindex methode, fmindex_collection:: nicht notwendig 

constexpr size_t Sigma = 5; // sigma steht für alphabetgröße

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

std::vector<uint8_t> reverse_complement_generator(std::vector<uint8_t>& ref){
    size_t i = size(ref)-1;
    std::vector<uint8_t> reverse_complement = {};
    
    while(i > 0){
        
        switch(ref[i]) {
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
            fmt::print("count error");
        }
        i--;

    }

    return reverse_complement;
}

int main(int argc, char const* const* argv) {
    (void)argc; // (void) sind nur für die strikten regeln des compilers
    (void)argv;
    auto reference = std::vector<std::vector<uint8_t>>{{4, 4, 4, 4, 1, 2, 2, 2, 1, 2, 3, 4, 4, 4, 4}}; // unser  Text/Referenz - Vektor mit {T, T, T, T, A, C, C, C, A, C, G, T, T, T, T} 

    for(size_t i = 0; i < reference.size(); i++){
        std::vector<uint8_t> ref = reference[i];
        std::vector<uint8_t> rev_ref = reverse_complement_generator(ref); // rev_ref existiert zur übersichtlichkeit 
        reference[i] = rev_ref; //kann man alternativ effektiver mit pointern lösen..
    }

    {
        std::cout << "\nBiFMIndex:\n";
        auto index = BiFMIndex<String<Sigma>>{reference, /*samplingRate*/16, /*threadNbr*/1};
        auto queries = std::vector<std::vector<uint8_t>>{{3, 4, 3}, {2, 1, 2}}; // unser Pattern/Read - Vektor {G, T, G} {C, A, C}

        search_backtracking::search(index, queries, 0, [&](size_t queryId, auto cursor, size_t errors) {
            (void)errors;
            fmt::print("found something {} {}\n", queryId, cursor.count()); // cursor.count() == range, range begriff wird verwechselt, print fmt kombiniert printf und stdcout
            for (auto i : cursor) {  
                auto [chr, pos] = index.locate(i); // curs
                fmt::print("chr/pos: {}/{}\n", chr, pos);
            }
        });
    }

    return 0;
}
