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

# --- Boost Setup ---
find_package(Boost QUIET CONFIG COMPONENTS beast asio system)

set(USE_SYSTEM_BOOST FALSE)
if(Boost_FOUND)

    set(BOOST_ALL_COMPONENTS_FOUND TRUE)
    foreach(COMPONENT beast asio system)
        if(NOT TARGET Boost::${COMPONENT})
            set(BOOST_ALL_COMPONENTS_FOUND FALSE)
            message(STATUS "System Boost found, but component Boost::${COMPONENT} target is missing.")
            break()
        endif()
        get_target_property(IS_IMPORTED Boost::${COMPONENT} IMPORTED)
        get_target_property(IS_INTERFACE Boost::${COMPONENT} INTERFACE_LIBRARY) 
                                                                             
        if(NOT IS_IMPORTED AND NOT IS_INTERFACE) 

        endif()
    endforeach()
    if(BOOST_ALL_COMPONENTS_FOUND AND TARGET Boost::beast AND TARGET Boost::asio AND TARGET Boost::system)  
        message(STATUS "System Boost found (${Boost_VERSION_STRING}) with required component targets (beast, asio, system).")
        message(STATUS "Using system Boost. SSL support depends on this installation.")
        set(USE_SYSTEM_BOOST TRUE)
    else()
        message(STATUS "System Boost found (${Boost_VERSION_STRING}), but not all required component targets (Boost::beast, Boost::asio, Boost::system) are adequately defined or found. Falling back to FetchContent.")
    endif()
else()
    message(STATUS "System Boost (version >= 1.74.0 with beast, asio, system) not found. Falling back to FetchContent.")
endif()

if(NOT USE_SYSTEM_BOOST)
    message(STATUS "Boost: Using FetchContent to download and build required components.")
    include(FetchContent)

    set(BOOST_INCLUDE_LIBRARIES "beast;asio;system" CACHE STRING "Boost libraries to build")
    set(BUILD_TESTING OFF CACHE BOOL "Disable Boost testing" FORCE) 
    set(BOOST_BUILD_TESTS OFF CACHE BOOL "Disable Boost testing" FORCE) 
    set(BOOST_BUILD_EXAMPLES OFF CACHE BOOL "Disable Boost examples" FORCE) 
    set(BOOST_ASIO_ENABLE_SSL OFF CACHE BOOL "Disable SSL in fetched Boost.Asio" FORCE)

    FetchContent_Declare(
        BoostSource
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG        boost-1.88.0
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
                                            
    )
    message(STATUS "Fetching and configuring Boost (this may take a while)... Please wait.")
    FetchContent_MakeAvailable(BoostSource)

    if(NOT (TARGET Boost::beast AND TARGET Boost::asio AND TARGET Boost::system))
        message(FATAL_ERROR "CRITICAL: Required Boost targets (Boost::beast, Boost::asio, Boost::system) were NOT created after FetchContent. Check CMake output for Boost build errors.")
    else()
        message(STATUS "Boost (beast, asio, system) successfully fetched and configured via FetchContent (SSL disabled for fetched Asio).")
    endif()
endif()

# --- MongoDB C++ Driver Setup ---
find_package(MongoCXX QUIET) 

if(NOT MongoCXX_FOUND OR NOT TARGET MongoCXX::mongocxx OR NOT TARGET MongoCXX::bsoncxx)
    if(MongoCXX_FOUND)
        message(STATUS "MongoDB C++ driver found, but required targets (MongoCXX::mongocxx, MongoCXX::bsoncxx) are missing. Falling back to FetchContent.")
    else()
        message(STATUS "MongoDB C++ driver (mongocxx) not found. Falling back to FetchContent.")
    endif()

    include(FetchContent)
 
    set(ENABLE_SSL OFF CACHE BOOL "Disable SSL for fetched mongocxx" FORCE)
    set(BSONCXX_POLY_USE_BOOST OFF CACHE BOOL "Do not use Boost for polyfills in fetched bsoncxx" FORCE) 
 
    set(FETCHCONTENT_ORIGINAL_BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}")
    set(BUILD_SHARED_LIBS OFF)

    FetchContent_Declare(
        mongocxxdriver
        GIT_REPOSITORY https://github.com/mongodb/mongo-cxx-driver.git
        GIT_TAG        r4.1.0 
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
    )
    message(STATUS "Fetching and configuring MongoDB C++ driver (this may take a while)... Please wait.")
    FetchContent_MakeAvailable(mongocxxdriver)

    
    set(BUILD_SHARED_LIBS "${FETCHCONTENT_ORIGINAL_BUILD_SHARED_LIBS}")
    unset(FETCHCONTENT_ORIGINAL_BUILD_SHARED_LIBS) 

    if(TARGET mongocxx_static AND NOT TARGET MongoCXX::mongocxx)
        add_library(MongoCXX::mongocxx ALIAS mongocxx_static)
        message(STATUS "Created ALIAS MongoCXX::mongocxx for mongocxx_static.")
    endif()
    if(TARGET bsoncxx_static AND NOT TARGET MongoCXX::bsoncxx)
        add_library(MongoCXX::bsoncxx ALIAS bsoncxx_static)
        message(STATUS "Created ALIAS MongoCXX::bsoncxx for bsoncxx_static.")
    endif()

    if(TARGET MongoCXX::mongocxx AND TARGET MongoCXX::bsoncxx)
        message(STATUS "MongoDB C++ driver (MongoCXX::mongocxx, MongoCXX::bsoncxx) successfully configured via FetchContent.")
        set(MongoCXX_FOUND TRUE CACHE INTERNAL "MongoDB C++ driver fetched and configured")
    else()
        message(WARNING "MongoDB C++ driver was fetched, but the expected targets (MongoCXX::mongocxx, MongoCXX::bsoncxx) are still not available. Linking may fail.")
    endif()
else()
    message(STATUS "Using system MongoDB C++ driver: ${MongoCXX_VERSION}")
endif()


# --- Application ---
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/**/*.cpp"
)
if(NOT APP_SOURCES)
    message(WARNING "No source .cpp files found in 'src' directory.")
endif()
add_executable(rudnik_app ${APP_SOURCES})

if(WIN32)
    target_compile_definitions(rudnik_app PRIVATE _WIN32_WINNT=0x0A00)
endif()

target_include_directories(rudnik_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(rudnik_app PRIVATE
    Boost::beast
    Boost::asio
    Boost::system
    MongoCXX::mongocxx
    MongoCXX::bsoncxx
)