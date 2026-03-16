cmake_minimum_required(VERSION 3.20)
project(TCI)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-only project
if(NOT WIN32)
    message(FATAL_ERROR "This project is Windows-only. Please use a Windows build environment.")
endif()

# Compiler detection and configuration
if(MSVC)
    # MSVC compiler
    add_compile_options(/W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # MinGW/GCC compiler
    add_compile_options(-Wall -Wextra)
    # Note: Some MSVC-specific features (__try/__except) may not work with GCC
    message(WARNING "GCC/MinGW detected. Some MSVC-specific features may not be available.")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Clang compiler
    add_compile_options(-Wall -Wextra)
    # Note: Some MSVC-specific features (__try/__except) may not work with Clang
    message(WARNING "Clang detected. Some MSVC-specific features may not be available.")
endif()

include(FetchContent)

# Safetyhook
FetchContent_Declare(
    safetyhook
    GIT_REPOSITORY "https://github.com/cursey/safetyhook.git"
    GIT_TAG "origin/main"
)
FetchContent_MakeAvailable(safetyhook)

# Build MinHook as static library
#file(GLOB MINHOOK_SRC 
#    "deps/minhook/src/*.c"
#    "deps/minhook/src/hde/*.c"
#)

# ImGui source files
file(GLOB IMGUI_SRC
    "deps/imgui/imgui.cpp"
    "deps/imgui/imgui_draw.cpp"
    "deps/imgui/imgui_tables.cpp"
    "deps/imgui/imgui_widgets.cpp"
    "deps/imgui/backends/imgui_impl_win32.cpp"
    "deps/imgui/backends/imgui_impl_dx9.cpp"
)

# Lua source files
file(GLOB LUA_SRC
    "deps/lua/lapi.c"
    "deps/lua/lauxlib.c"
    "deps/lua/lbaselib.c"
    "deps/lua/lcode.c"
    "deps/lua/lcorolib.c"
    "deps/lua/lctype.c"
    "deps/lua/ldblib.c"
    "deps/lua/ldebug.c"
    "deps/lua/ldo.c"
    "deps/lua/ldump.c"
    "deps/lua/lfunc.c"
    "deps/lua/lgc.c"
    "deps/lua/linit.c"
    "deps/lua/liolib.c"
    "deps/lua/llex.c"
    "deps/lua/lib.c"
    "deps/lua/lmathlib.c"
    "deps/lua/lmem.c"
    "deps/lua/loadlib.c"
    "deps/lua/lobject.c"
    "deps/lua/lopcodes.c"
    "deps/lua/loslib.c"
    "deps/lua/lparser.c"
    "deps/lua/lstate.c"
    "deps/lua/lstring.c"
    "deps/lua/lstrlib.c"
    "deps/lua/ltable.c"
    "deps/lua/ltablib.c"
    "deps/lua/ltests.c"
    "deps/lua/ltm.c"
    "deps/lua/lua.c"
    "deps/lua/lundump.c"
    "deps/lua/lutf8lib.c"
    "deps/lua/lvm.c"
    "deps/lua/lzio.c"
    # "deps/lua/onelua.c"
)

set(CMAKE_BUILD_TYPE Debug)

add_library(TCI_DLL SHARED
    src/bridge-layout.cpp
    src/bridge.cpp
    src/detour.cpp
    src/dll-lua.cpp
    src/dllmain.cpp
    src/gui.cpp
    src/lua-api.cpp
    src/lua-state.cpp
    src/magic-call.cpp
    src/pattern.cpp
    src/tci-api.cpp
    src/utils.cpp
    ${LUA_SRC}
    ${IMGUI_SRC}
)

target_compile_options(TCI_DLL PRIVATE $<$<CONFIG:Debug>:/Zi>) # MSVC
# target_compile_options(your_dll PRIVATE $<$<CONFIG:Debug>:-g>) # GCC/Clang

target_link_libraries(TCI_DLL PRIVATE safetyhook)

target_include_directories(TCI_DLL PRIVATE
    deps/imgui
    deps
    src
)

target_link_libraries(TCI_DLL PRIVATE
    d3d9.lib 
)

add_executable(AutoInjector src/auto-injector.cpp)