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
find_package(Boost 1.74.0 QUIET COMPONENTS beast asio system)
if(NOT Boost_FOUND)
    message(STATUS "Boost ni najden, prenašanje in prevajanje potrebnih komponent za HTTP...")
    include(FetchContent)
    set(BOOST_INCLUDE_LIBRARIES "beast;asio;system" CACHE STRING "")
    set(BUILD_TESTING OFF CACHE BOOL "")
    set(BOOST_BUILD_TESTS OFF CACHE BOOL "")
    set(BOOST_BUILD_EXAMPLES OFF CACHE BOOL "")
    set(BOOST_ASIO_ENABLE_SSL OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        BoostSource
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG        boost-1.88.0
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
        GIT_SUBMODULE_UPDATE_FLAGS "--depth;1"
    )
    message(STATUS "Prenašanje in konfiguriranje Boost (to lahko traja dolgo)... Prosimo, počakajte.")
    FetchContent_MakeAvailable(BoostSource)
    if(NOT (TARGET Boost::beast AND TARGET Boost::asio AND TARGET Boost::system))
        message(FATAL_ERROR "Napaka: Ena ali več zahtevanih Boost tarč (Beast, Asio, System) NI bilo ustvarjenih po prenosu. Preverite izpis.")
    else()
        message(STATUS "Minimalni Boost (Beast, Asio, System) prenesen in konfiguriran (brez SSL).")
    endif()
else()
    message(STATUS "Najden Boost: ${Boost_VERSION_STRING}")
    message(STATUS "Uporablja se sistemski Boost. Podpora za SSL v Boost.Asio je odvisna od te namestitve.")
endif()

# --- MongoDB C++ Driver Setup ---
find_package(MongoCXX QUIET)
if(NOT MongoCXX_FOUND)
    message(STATUS "MongoDB C++ gonilnik (mongocxx) ni najden, prenašanje in prevajanje...")
    include(FetchContent)
    set(ENABLE_SSL OFF CACHE BOOL "" FORCE)
    set(BSONCXX_POLY_USE_BOOST OFF CACHE BOOL "" FORCE)
    set(FETCHCONTENT_ORIGINAL_BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}")
    set(BUILD_SHARED_LIBS OFF)
    FetchContent_Declare(
        mongocxxdriver
        GIT_REPOSITORY https://github.com/mongodb/mongo-cxx-driver.git
        GIT_TAG        r4.1.0
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
    )
    message(STATUS "Prenašanje in konfiguriranje MongoDB C++ gonilnika (to lahko traja dolgo)... Prosimo, počakajte.")
    FetchContent_MakeAvailable(mongocxxdriver)
    set(BUILD_SHARED_LIBS "${FETCHCONTENT_ORIGINAL_BUILD_SHARED_LIBS}")
    unset(FETCHCONTENT_ORIGINAL_BUILD_SHARED_LIBS)
    if(TARGET MongoCXX::mongocxx AND TARGET MongoCXX::bsoncxx)
        message(STATUS "MongoDB C++ gonilnik (MongoCXX::mongocxx, MongoCXX::bsoncxx) ALIAS tarče najdene.")
        set(MongoCXX_FOUND TRUE CACHE INTERNAL "")
    elseif(TARGET mongocxx_static AND TARGET bsoncxx_static)
        message(STATUS "MongoDB C++ gonilnik (mongocxx_static, bsoncxx_static) dejanske STATIČNE tarče najdene. Ustvarjanje ALIASov...")
        add_library(MongoCXX::mongocxx ALIAS mongocxx_static)
        add_library(MongoCXX::bsoncxx ALIAS bsoncxx_static)
        set(MongoCXX_FOUND TRUE CACHE INTERNAL "")
    else()
        message(WARNING "OPOZORILO: Pričakovane MongoDB C++ gonilniške tarče (MongoCXX::* ali *_static) niso bile jasno identificirane po FetchContent_MakeAvailable. Povezovanje bo poskušeno, vendar lahko spodleti.")
        set(MongoCXX_FOUND TRUE CACHE INTERNAL "MongoDB C++ driver fetched, target visibility unclear")
    endif()
else()
    message(STATUS "Najden MongoDB C++ gonilnik: ${MongoCXX_VERSION}")
endif()

# --- Application ---
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/**/*.cpp"
)
if(NOT APP_SOURCES)
    message(WARNING "Nobena izvorna .cpp datoteka ni bila najdena v mapi 'src'.")
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
)#