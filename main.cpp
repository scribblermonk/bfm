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
#include <clice/clice.h>
#include <ivio/ivio.h>

using namespace fmindex_collection; //for all of the fmindex methods
    
auto cliReduced = clice::Argument{ .args   = {"-r", "--reduced"},
                                .desc   = "-r, --reduced enter reduced mode",
                                };

auto cliHelp    = clice::Argument{ .args   = "--help", .desc   = "prints the help page", .cb     = [](){ std::cout << clice::generateHelp(); exit(0); }, .tags   = {"ignore-required"}, };
// generates help page

constexpr size_t Sigma = 5; // Sigma == Alphabet size
constexpr size_t reduced_Sigma = 3; // reduced Sigma == reduced alphabetsize

template <size_t T_Sigma>
using String = string::InterleavedBitvector16<T_Sigma>; // template/"function call"

template <typename Index> // saves index
void saveIndex(Index const& _index, std::filesystem::path _fileName) { 
    auto ofs     = std::ofstream(_fileName, std::ios::binary);
    auto archive = cereal::BinaryOutputArchive{ofs};
    archive(_index);
}

template <typename Index> // loads index
auto loadIndex(std::filesystem::path _fileName) {
    auto ifs     = std::ifstream(_fileName, std::ios::binary);
    auto archive = cereal::BinaryInputArchive{ifs};
    auto index = Index{};
    archive(index);
    return index;
}

std::vector<uint8_t> letter_to_number(std::string_view seq){ // string_view for minimal copy 
    std::vector<uint8_t> num_seq = {};
    for(auto letter : seq)
    {
        switch(letter) { 
            case 'A':
            num_seq.push_back(1);
            break;

            case 'C':
            num_seq.push_back(2);
            break;

            case 'G':
            num_seq.push_back(3);
            break;

            case 'T':
            num_seq.push_back(4);
            break;

            default:
            num_seq.push_back(1); //erstmal A wollen aber schön mit eigentlich weitermachen rnd int  
        }
    }
    return num_seq;
} 

std::vector<uint8_t> reduction(std::vector<uint8_t> const& ref){ // {1,4,1,3,2} -> {1,1,1,2,2}
    std::vector<uint8_t> reduced = {}; 
    reduced.reserve(size(ref)); 
    for (const uint8_t& i: ref)
        {
        switch(i) { // basically a look up table 
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

std::vector<std::vector<uint8_t>> reverse_generator(std::vector<std::vector<uint8_t>> chromosomes){ // {1,1,1,2,2} -> {1,1,1,2,2,0,2,2,1,1,1} 
    for(std::vector<uint8_t>& ref : chromosomes){
        std::vector<uint8_t> reverse_ref = ref;
        std::reverse(reverse_ref.begin(), reverse_ref.end());
        ref.push_back(0); 
        ref.insert(ref.end(), reverse_ref.begin(), reverse_ref.end());
        // fmt::print("{} \n", ref); //debugging 
    }
    return chromosomes;
}

std::vector<std::vector<uint8_t>> reverse_complement_generator(std::vector<std::vector<uint8_t>> chromosomes){  // {{1,4,1,3,2}} ->  {{1,4,1,3,2,0,2,3,1,4,1}}
    
    for(std::vector<uint8_t>& ref : chromosomes){    // call by reference critical for functionality
        std::vector<uint8_t> reverse_complement = {}; 
        reverse_complement.reserve(size(ref)); 

        for (const uint8_t& i: ref | std::views::reverse) // fancy method to generate revers
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
        ref.push_back(0);
        ref.insert(ref.end(), reverse_complement.begin(), reverse_complement.end());  // { a, b, 0} -> { a, b, 0, b, a}
        //fmt::print("{} \n", ref); //debugging 
    }
    return chromosomes;
}

int main(int argc, char const* const* argv) {
    (void)argc; // (void) sind nur für die strikten regeln des compilers
    (void)argv;

    std::ofstream hit_log;
    hit_log.open("../hit_log.txt");

    if (auto failed = clice::parse(argc, argv); failed) {
        std::cerr << "parsing failed: " << *failed << "\n";
        return 1;
    }
    
    bool reduced = cliReduced; 

    auto queries = std::vector<std::vector<uint8_t>>{}; // {3, 4, 3}, {2, 1, 2} unser Pattern/Read - Vektor {G, T, G} {C, A, C}
    //fasta input Queries
    auto inputQueries = std::filesystem::path{"../quer.txt"};
    auto quer_reader = ivio::fasta::reader{{.input = inputQueries}};
    for (auto record_view : quer_reader) {
        queries.push_back(letter_to_number(record_view.seq)); //pushback of converted text string 
    }

    auto chromosomes = std::vector<std::vector<uint8_t>>{}; // {4, 4, 4, 4, 1, 2, 2, 2, 1, 2, 3, 4, 4, 4, 4} unser  Text/Referenz - Vektor mit {T, T, T, T, A, C, C, C, A, C, G, T, T, T, T} 
    //fasta input Reference
    auto inputRef = std::filesystem::path{"../ref.txt"};
    auto ref_reader = ivio::fasta::reader{{.input = inputRef}};
    for (auto record_view : ref_reader) {
        chromosomes.push_back(letter_to_number(record_view.seq));
    }
    
    // ohne reduction
    if(!reduced){
        std::cout << "running BiFMindex 1.0 (not reduced)\n";
        auto chromosomes_with_complement = chromosomes; 
        bool reduced = false;

        chromosomes_with_complement = reverse_complement_generator(chromosomes_with_complement);
        
        // for(auto ref : chromosomes_with_complement){
        //     fmt::print("{} \n", ref);
        //  } // debugging

        auto index = BiFMIndex<String<Sigma>>{chromosomes_with_complement, /*samplingRate*/16, /*threadNbr*/1};

        search_backtracking::search(index, queries, 0, [&](size_t queryId, auto cursor, size_t errors) {
            (void) errors;
            for (auto i : cursor) {  
                auto [chr, pos] = index.locate(i); // cursor stuff
                 hit_log << fmt::format("query_index/chromosome_index/position_of_hit: {}/{}/{}\n", queryId, chr, pos);  // cursor.count() == range, range term is confusing, print fmt combines printf and stdcout
            }
        });
    }

    // mit reduction
    if(reduced){
        std::cout << "running reduced BiFMindex 2.0 (reduced)\n";
        auto reduced_chromosomes = chromosomes;

        for (size_t i = 0; i < size(reduced_chromosomes); i++){ 
            reduced_chromosomes[i] = reduction(reduced_chromosomes[i]); // extraloop 
        }
        
        reduced_chromosomes = reverse_generator(reduced_chromosomes);

        std::vector<std::vector<uint8_t>> reduced_queries;
        for(std::vector<uint8_t> query : queries){
            reduced_queries.push_back(reduction(query));
        }
        
        auto reduced_index = BiFMIndex<String<reduced_Sigma>>{reduced_chromosomes, /*samplingRate*/16, /*threadNbr*/1};

        search_backtracking::search(reduced_index, reduced_queries, 0, [&](size_t queryId, auto cursor, size_t errors) {
            (void) errors;
            for (auto i : cursor) {  
                auto [chr, pos] = reduced_index.locate(i); // cursor stuff
                hit_log << fmt::format("query_index/chromosome_index/position_of_hit: {}/{}/{}\n", queryId, chr, pos);  // cursor.count() == range, range term is confusing, print fmt combines printf and stdcout
            }
        });
    }
    return 0; 
}