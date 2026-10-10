cmake_minimum_required(VERSION 3.20)
project(TCISDK_Project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(TCISDK STATIC)

target_sources(TCISDK PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sdk.cpp"
)

target_include_directories(TCISDK PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(TCISDK PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/TCI_DLL.lib"
)

option(TCI_BUILD_EXAMPLES "Build SDK example" OFF)

if(TCI_BUILD_EXAMPLES)
    message(STATUS "[TCISDK] Building example enabled.")

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/example/simple/main.cpp")
        add_library(TCISimpleExtension SHARED
            "${CMAKE_CURRENT_SOURCE_DIR}/example/simple/main.cpp"
        )

        target_compile_features(TCISimpleExtension PRIVATE cxx_std_20)
        target_link_libraries(TCISimpleExtension PRIVATE TCISDK)
        set_target_properties(TCISimpleExtension PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/extensions"
        )
    endif()
endif()