cmake_minimum_required (VERSION 3.16)

project (bfm VERSION 1.0.0)

# Force language standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(OpenMP QUIET)
include(cmake/CPM.cmake)
CPMAddPackage("gh:SGSSGene/fmindex-collection#comp/awfmindex")
CPMAddPackage("gh:fmtlib/fmt@11.2.0#11.2.0")
CPMAddPackage("gh:SGSSGene/clice@1.1.1")
CPMAddPackage("gh:iv-project/IVio@0.4.3")


add_executable(bfm
    main.cpp
)

target_link_libraries(bfm
    fmindex-collection::fmindex-collection
    fmt::fmt
    clice::clice
    ivio::ivio
)