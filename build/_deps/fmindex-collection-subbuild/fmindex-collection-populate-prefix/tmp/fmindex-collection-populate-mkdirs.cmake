# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/nikolai/Documents/bfm/build/_deps/fmindex-collection-src"
  "/home/nikolai/Documents/bfm/build/_deps/fmindex-collection-build"
  "/home/nikolai/Documents/bfm/build/_deps/fmindex-collection-subbuild/fmindex-collection-populate-prefix"
  "/home/nikolai/Documents/bfm/build/_deps/fmindex-collection-subbuild/fmindex-collection-populate-prefix/tmp"
  "/home/nikolai/Documents/bfm/build/_deps/fmindex-collection-subbuild/fmindex-collection-populate-prefix/src/fmindex-collection-populate-stamp"
  "/home/nikolai/Documents/bfm/build/_deps/fmindex-collection-subbuild/fmindex-collection-populate-prefix/src"
  "/home/nikolai/Documents/bfm/build/_deps/fmindex-collection-subbuild/fmindex-collection-populate-prefix/src/fmindex-collection-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/nikolai/Documents/bfm/build/_deps/fmindex-collection-subbuild/fmindex-collection-populate-prefix/src/fmindex-collection-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/nikolai/Documents/bfm/build/_deps/fmindex-collection-subbuild/fmindex-collection-populate-prefix/src/fmindex-collection-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
