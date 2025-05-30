file(GLOB_RECURSE CORE_APP_SOURCES CONFIGURE_DEPENDS
        "*.cpp"
        "**/*.cpp"
)


set(MAIN_APP_ENTRY_POINT_FILE "index.cpp"
)
list(FILTER CORE_APP_SOURCES EXCLUDE REGEX ".*/${MAIN_APP_ENTRY_POINT_FILE}$")

if(NOT CORE_APP_SOURCES)
    message(WARNING "Nobena izvorna .cpp datoteka (razen morda ${MAIN_APP_ENTRY_POINT_FILE}) ni bila najdena za knjižnico RudnikCore v mapi 'src'.")
endif()

message(STATUS "Datoteke, vključene v RudnikCore: ${CORE_APP_SOURCES}")

add_library(RudnikCore STATIC
        ${CORE_APP_SOURCES}
)

target_include_directories(RudnikCore PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../includes
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(RudnikCore PUBLIC
        Boost::system
        ${MONGO_BSONCXX_LIBRARY}
        ${MONGO_MONGOCXX_LIBRARY}
        jwt-cpp::jwt-cpp
)

target_compile_definitions(RudnikCore PRIVATE NOMINMAX)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${MAIN_APP_ENTRY_POINT_FILE}")
    add_executable(rudnik_app
            ${MAIN_APP_ENTRY_POINT_FILE} # e.g., main.cpp
    )

    if(WIN32)
        target_compile_definitions(rudnik_app PRIVATE _WIN32_WINNT=0x0A00)
    endif()

    target_link_libraries(rudnik_app PRIVATE
            RudnikCore
    )

    target_link_libraries(rudnik_app PRIVATE jwt-cpp::jwt-cpp)

    include_directories(rudnik_app PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../build/_deps/jwt-cpp-src/include")
    include_directories(rudnik_app PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../out/build/x64-debug/_deps/jwt-cpp-src/include")

    target_include_directories(rudnik_app PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../includes
            ${CMAKE_CURRENT_SOURCE_DIR}
    )
else()
    message(WARNING "Glavna .cpp datoteka aplikacije ('${MAIN_APP_ENTRY_POINT_FILE}') ni bila najdena v mapi 'src'. Izvršljiva datoteka 'rudnik_app' ne bo ustvarjena.")
endif()