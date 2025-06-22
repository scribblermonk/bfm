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
#include <sstream>

using namespace fmindex_collection; // für die ganzen fmindex methode, fmindex_collection:: nicht notwendig 

constexpr size_t Sigma = 6; // Sigma steht für alphabetgröße

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

std::vector<uint8_t> reduction(std::vector<uint8_t> const& ref){
    std::vector<uint8_t> reduced = {}; 
    reduced.reserve(size(ref)); 
    for (const uint8_t& i: ref)
        {
        switch(i) {
                case 3:
                reduced.push_back(2);
                break;

                case 4:
                reduced.push_back(1);
                break;

                default:
                reduced.push_back(i);
        }
        }  
    return reduced;
}

std::vector<std::vector<uint8_t>> reverse_generator(std::vector<std::vector<uint8_t>> chromosomes){
    for(std::vector<uint8_t>& ref : chromosomes){
        std::vector<uint8_t> reverse_ref = ref;
        std::reverse(reverse_ref.begin(), reverse_ref.end());
        ref.push_back(5);
        ref.insert(ref.end(), reverse_ref.begin(), reverse_ref.end());
        // fmt::print("{} \n", ref); //debugging 
    }
    return chromosomes;
}

std::vector<std::vector<uint8_t>> reverse_complement_generator(std::vector<std::vector<uint8_t>> chromosomes){    
    
    for(std::vector<uint8_t>& ref : chromosomes){    // call by reference critical for functionality
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
        ref.push_back(5);
        ref.insert(ref.end(), reverse_complement.begin(), reverse_complement.end());  // { a, b, 5} -> { a, b, 5, b, a}
        //fmt::print("{} \n", ref); //debugging 
    }
    return chromosomes;
}

// std::vector<std::vector<uint8_t>> nameless(std::vector<std::vector<uint8_t>> chromosomes){
//     for (size_t i = 0; i < size(chromosomes); i++){ // iterator durch chromosomes
//         std::vector<uint8_t> temp = reverse_complement_generator(chromosomes[i]);
//         chromosomes[i].push_back(5); // { a, b} -> { a, b, 5} // nicht vergessen in der Main möglicherweise einfach dran zu hängen
//         chromosomes[i].insert(chromosomes[i].end(), temyp.begin(), temp.end());  // { a, b, 5} -> { a, b, 5, b, a}
//         fmt::print("{} \n", chromosomes[i]);
//     }
//     return chromosomes;
// }

int main(int argc, char** argv []) {
    // (void)argc; // (void) sind nur für die strikten regeln des compilers
    // (void)argv;
    
    std::stringstream ss(argv[1]);
    bool reduced;
    ss >> std::boolalpha >> reduced;

    auto queries = std::vector<std::vector<uint8_t>>{{3, 4, 3}, {2, 1, 2}}; // unser Pattern/Read - Vektor {G, T, G} {C, A, C}
    auto chromosomes = std::vector<std::vector<uint8_t>>{{4, 4, 4, 4, 1, 2, 2, 2, 1, 2, 3, 4, 4, 4, 4}}; // unser  Text/Referenz - Vektor mit {T, T, T, T, A, C, C, C, A, C, G, T, T, T, T} 
    
    // ohne reduction
    if(!reduced){
        std::cout << "\nNot Reduced\n";
        auto chromosomes_with_complement = chromosomes; 

        chromosomes_with_complement = reverse_complement_generator(chromosomes_with_complement);
        
        for(auto ref : chromosomes_with_complement){
            fmt::print("{} \n", ref);
        }

        std::cout << "\nBiFMIndex:\n";
        auto index = BiFMIndex<String<Sigma>>{chromosomes_with_complement, /*samplingRate*/16, /*threadNbr*/1};

        search_backtracking::search(index, queries, 0, [&](size_t queryId, auto cursor, size_t errors) {
            (void) errors;
            fmt::print("found something {} {}\n", queryId, cursor.count()); // cursor.count() == range, range begriff wird verwechselt, print fmt kombiniert printf und stdcout
            for (auto i : cursor) {  
                auto [chr, pos] = index.locate(i); // cursor stuff
                fmt::print("chr/pos: {}/{}\n", chr, pos);
            }
        });
    }

    // mit reduction
    if(reduced){
        std::cout << "\nReduced\n";
        auto reduced_chromosomes = chromosomes;

        for (size_t i = 0; i < size(reduced_chromosomes); i++){ 
            reduced_chromosomes[i] = reduction(reduced_chromosomes[i]); // einfach nur mit dieser extra for loop
        }
        
        reduced_chromosomes = reverse_generator(reduced_chromosomes);

        std::vector<std::vector<uint8_t>> reduced_queries;
        for(std::vector<uint8_t> query : queries){
            reduced_queries.push_back(reduction(query));
        }

        std::cout << "\nBiFMIndex:\n";
        auto reduced_index = BiFMIndex<String<Sigma>>{reduced_chromosomes, /*samplingRate*/16, /*threadNbr*/1};

        search_backtracking::search(reduced_index, reduced_queries, 0, [&](size_t queryId, auto cursor, size_t errors) {
            (void) errors;
            fmt::print("found something {} {}\n", queryId, cursor.count()); // cursor.count() == range, range begriff wird verwechselt, print fmt kombiniert printf und stdcout
            for (auto i : cursor) {  
                auto [chr, pos] = reduced_index.locate(i); // cursor stuff
                fmt::print("chr/pos: {}/{}\n", chr, pos);
            }
        });

    }

    else{
    }
    return 0; 
}

// Aktueller Plan und Fragen
// Main mit 0-1 um reduction an und aus zu stellen
