include("/home/nikolai/Documents/bfm/build/cmake/CPM_0.40.2.cmake")
CPMAddPackage("NAME;libsais;VERSION;2.8.6;GITHUB_REPOSITORY;IlyaGrebnov/libsais;SYSTEM;YES;EXCLUDE_FROM_ALL;YES;OPTIONS;LIBSAIS_USE_OPENMP TRUE;LIBSAIS_BUILD_SHARED_LIB OFF")
set(libsais_FOUND TRUE)