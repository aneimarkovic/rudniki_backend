cmake_minimum_required(VERSION 3.16)
project(rudniki-backend LANGUAGES CXX)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()
if(POLICY CMP0148)
  cmake_policy(SET CMP0148 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(CMAKE_FIND_PACKAGE_SORT_ORDER NATURAL)
SET(CMAKE_FIND_PACKAGE_SORT_DIRECTION DEC)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(Boost 1.74.0 QUIET COMPONENTS system) 
else()
    find_package(Boost 1.88 QUIET COMPONENTS system) 
endif()

if(NOT Boost_FOUND)
    message(STATUS "Required Boost components (system, beast, asio) not found via find_package.")
    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        message(FATAL_ERROR "Boost not found. On macOS, you can typically install Boost (including system, beast, asio components) via Homebrew (e.g., 'brew install boost'). Please consult the README.md for detailed installation instructions and then re-run CMake.")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        message(FATAL_ERROR "Boost not found. On Windows, please install the required Boost components (system, beast, asio). Refer to the README.md for installation instructions, then re-run CMake.")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        message(FATAL_ERROR "Boost not found. On Linux, please install the Boost development packages (e.g., for Debian/Ubuntu: 'sudo apt update && sudo apt install libboost-system-dev libboost-beast-dev libboost-asio-dev'). Refer to the README.md for more details and then re-run CMake.")
    else()
        message(FATAL_ERROR "Boost not found. Please install Boost (including system, beast, asio components) for your system (${CMAKE_SYSTEM_NAME}). Refer to the README.md for installation instructions, then re-run CMake.")
    endif()
endif()

message(STATUS "Found Boost: ${Boost_VERSION_STRING}")
if(NOT Boost_SYSTEM_FOUND)
    message(FATAL_ERROR "Boost 'system' component was not found. Please ensure your Boost installation includes it.")
endif()

message(STATUS "Required Boost components (system, beast, asio) successfully located.")

if(NOT TARGET Boost::system)
    message(FATAL_ERROR "CMake target Boost::system is not available. Your Boost installation might not be correctly configured for CMake.")
endif()

find_package(bsoncxx CONFIG QUIET)
if(NOT bsoncxx_FOUND)
    message(FATAL_ERROR "bsoncxx (del MongoDB C++ gonilnika) ni bil najden. Prosimo, preverite namestitev preko vcpkg in README.md.")
else()
    message(STATUS "Found bsoncxx (del MongoDB C++ gonilnika). Version: ${bsoncxx_VERSION}")
    if(NOT (TARGET mongo::bsoncxx_static OR TARGET mongo::bsoncxx_shared))
        message(FATAL_ERROR "CMake tarči mongo::bsoncxx_static ali mongo::bsoncxx_shared nista na voljo, čeprav je bil bsoncxx najden. Preverite vcpkg namestitev.")
    endif()
endif()

find_package(mongocxx CONFIG QUIET)
if(NOT mongocxx_FOUND)
    message(FATAL_ERROR "mongocxx (MongoDB C++ gonilnik) ni bil najden. Prosimo, preverite namestitev preko vcpkg in README.md.")
else()
    message(STATUS "Found mongocxx (MongoDB C++ gonilnik). Version: ${mongocxx_VERSION}")
    if(NOT (TARGET mongo::mongocxx_static OR TARGET mongo::mongocxx_shared))
        message(FATAL_ERROR "CMake tarči mongo::mongocxx_static ali mongo::mongocxx_shared nista na voljo, čeprav je bil mongocxx najden. Preverite vcpkg namestitev.")
    endif()
endif()

set(MONGO_BSONCXX_LIBRARY "$<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>")
set(MONGO_MONGOCXX_LIBRARY "$<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>")

message(STATUS "MongoDB C++ driver libraries configured as: BSON=${MONGO_BSONCXX_LIBRARY}, Driver=${MONGO_MONGOCXX_LIBRARY}")

add_subdirectory(src)
include_directories(includes)

find_package(Catch2 3 QUIET)

if(NOT Catch2_FOUND)
    message(FATAL_ERROR "System Catch2 v3 (or newer) not found. Please install Catch2 and ensure it can be found by CMake. Refer to the README.md for installation instructions for your platform.")
else()
    message(STATUS "Found system Catch2: ${Catch2_VERSION}")
    
    if(NOT TARGET Catch2::Catch2WithMain AND NOT TARGET Catch2::Catch2)
        message(FATAL_ERROR "Catch2 was found, but neither Catch2::Catch2WithMain nor Catch2::Catch2 CMake target is available. Your Catch2 installation might be incomplete or not correctly configured for CMake. Please check your Catch2 installation.")
    elseif(NOT TARGET Catch2::Catch2WithMain)
        message(WARNING "Catch2 target 'Catch2::Catch2WithMain' not found. Using 'Catch2::Catch2' instead. You will need to provide your own main() function in your test executable, or your tests might not link correctly if they expect Catch2 to provide main().")
    else()
        message(STATUS "Found Catch2 target: Catch2::Catch2WithMain.")
    endif()
endif()

find_package(OpenSSL REQUIRED)
if(OpenSSL_FOUND)
    message(STATUS "Found OpenSSL: ${OpenSSL_VERSION} (Include: ${OpenSSL_INCLUDE_DIR}, Libs: ${OpenSSL_LIBRARIES})")
else()
    message(FATAL_ERROR "OpenSSL not found. Please install OpenSSL development libraries. See README.md for instructions.")
endif()

include(FetchContent)
fetchcontent_declare(jwt-cpp 
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG 08bcf77a687fb06e34138e9e9fa12a4ecbe12332 # v0.7.0 release
)
set(JWT_BUILD_EXAMPLES OFF CACHE BOOL "disable building examples" FORCE)
fetchcontent_makeavailable(jwt-cpp)

find_package(unofficial-sodium CONFIG REQUIRED)

enable_testing()
add_subdirectory(test)
