# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/aneimarkovic/Documents/Feri/Feri_2_letnik/Projektni_praktikum/rudnik/rudniki_backend/cmake-build-debug/_deps/jwt-cpp-src")
  file(MAKE_DIRECTORY "/Users/aneimarkovic/Documents/Feri/Feri_2_letnik/Projektni_praktikum/rudnik/rudniki_backend/cmake-build-debug/_deps/jwt-cpp-src")
endif()
file(MAKE_DIRECTORY
  "/Users/aneimarkovic/Documents/Feri/Feri_2_letnik/Projektni_praktikum/rudnik/rudniki_backend/cmake-build-debug/_deps/jwt-cpp-build"
  "/Users/aneimarkovic/Documents/Feri/Feri_2_letnik/Projektni_praktikum/rudnik/rudniki_backend/cmake-build-debug/_deps/jwt-cpp-subbuild/jwt-cpp-populate-prefix"
  "/Users/aneimarkovic/Documents/Feri/Feri_2_letnik/Projektni_praktikum/rudnik/rudniki_backend/cmake-build-debug/_deps/jwt-cpp-subbuild/jwt-cpp-populate-prefix/tmp"
  "/Users/aneimarkovic/Documents/Feri/Feri_2_letnik/Projektni_praktikum/rudnik/rudniki_backend/cmake-build-debug/_deps/jwt-cpp-subbuild/jwt-cpp-populate-prefix/src/jwt-cpp-populate-stamp"
  "/Users/aneimarkovic/Documents/Feri/Feri_2_letnik/Projektni_praktikum/rudnik/rudniki_backend/cmake-build-debug/_deps/jwt-cpp-subbuild/jwt-cpp-populate-prefix/src"
  "/Users/aneimarkovic/Documents/Feri/Feri_2_letnik/Projektni_praktikum/rudnik/rudniki_backend/cmake-build-debug/_deps/jwt-cpp-subbuild/jwt-cpp-populate-prefix/src/jwt-cpp-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/aneimarkovic/Documents/Feri/Feri_2_letnik/Projektni_praktikum/rudnik/rudniki_backend/cmake-build-debug/_deps/jwt-cpp-subbuild/jwt-cpp-populate-prefix/src/jwt-cpp-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/aneimarkovic/Documents/Feri/Feri_2_letnik/Projektni_praktikum/rudnik/rudniki_backend/cmake-build-debug/_deps/jwt-cpp-subbuild/jwt-cpp-populate-prefix/src/jwt-cpp-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
