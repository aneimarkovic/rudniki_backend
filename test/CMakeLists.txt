

add_executable(RudnikAppTests
    RouterUtilTest.cpp 
)

target_link_libraries(RudnikAppTests PRIVATE
    RudnikCore          
    Catch2::Catch2WithMain
)

target_include_directories(RudnikAppTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../includes 
    ${CMAKE_CURRENT_SOURCE_DIR}/../src     
)


if(WIN32)
    target_compile_definitions(RudnikAppTests PRIVATE _WIN32_WINNT=0x0A00)
endif()

if(TARGET Catch2::Catch2WithMain)
    include(Catch)
    catch_discover_tests(RudnikAppTests)
else()
    message(WARNING "Catch2 main target not found. Tests for RudnikAppTests will not be discovered automatically.")
endif()