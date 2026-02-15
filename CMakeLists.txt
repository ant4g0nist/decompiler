cmake_minimum_required(VERSION 3.16)
project(lldbghidra LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ZLIB REQUIRED)

# ---- Find LLDB ----
# Bundled LLDB API headers (from llvm-project)
set(LLDB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src/include")
if(NOT EXISTS "${LLDB_INCLUDE_DIR}/lldb/API/LLDB.h")
    message(FATAL_ERROR "LLDB headers not found at ${LLDB_INCLUDE_DIR}. "
        "Run: git submodule update --init, or see README for setup instructions.")
endif()
message(STATUS "Using LLDB headers from: ${LLDB_INCLUDE_DIR}")

# Find LLDB.framework for linking
set(LLDB_FRAMEWORK_SEARCH_PATHS
    "/Applications/Xcode.app/Contents/SharedFrameworks"
    "/Applications/Xcode-beta.app/Contents/SharedFrameworks"
    "/Library/Developer/CommandLineTools/Library/PrivateFrameworks"
)

set(LLDB_FRAMEWORK_DIR "")
foreach(dir ${LLDB_FRAMEWORK_SEARCH_PATHS})
    if(EXISTS "${dir}/LLDB.framework")
        set(LLDB_FRAMEWORK_DIR "${dir}")
        break()
    endif()
endforeach()

# Fallback: xcode-select
if(NOT LLDB_FRAMEWORK_DIR)
    execute_process(
        COMMAND xcode-select -p
        OUTPUT_VARIABLE XCODE_DEV_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(XCODE_DEV_DIR)
        set(_dir "${XCODE_DEV_DIR}/../SharedFrameworks")
        if(EXISTS "${_dir}/LLDB.framework")
            get_filename_component(LLDB_FRAMEWORK_DIR "${_dir}" REALPATH)
        else()
            set(_dir "${XCODE_DEV_DIR}/Library/PrivateFrameworks")
            if(EXISTS "${_dir}/LLDB.framework")
                get_filename_component(LLDB_FRAMEWORK_DIR "${_dir}" REALPATH)
            endif()
        endif()
    endif()
endif()

if(LLDB_FRAMEWORK_DIR)
    message(STATUS "Found LLDB.framework at: ${LLDB_FRAMEWORK_DIR}")
else()
    message(WARNING "LLDB.framework not found. Plugin will compile but may not link correctly.")
    set(LLDB_FRAMEWORK_DIR "/Library/Developer/CommandLineTools/Library/PrivateFrameworks")
endif()

# ---- Build Ghidra decompiler ----
add_subdirectory(third_party/ghidra-decompiler)

# ---- Build lldbghidra plugin ----
add_library(lldbghidra SHARED
    src/Plugin.cpp
    src/ArchMap.cpp
    src/LLDBArchitecture.cpp
    src/LLDBLoadImage.cpp
    src/LLDBScope.cpp
    src/LLDBTypeFactory.cpp
    src/LLDBCommentDatabase.cpp
    src/LLDBPrintC.cpp
    src/EmitAnsi.cpp
    src/DecompileCommand.cpp
)

# Let the build configuration control asserts (CMake will define NDEBUG for Release).
target_compile_definitions(lldbghidra PRIVATE $<$<CONFIG:Release>:NDEBUG>)
target_include_directories(lldbghidra PRIVATE
    "${LLDB_INCLUDE_DIR}"
    "${CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(lldbghidra PRIVATE
    libdecomp_static
    ZLIB::ZLIB
)

# Link against LLDB framework
target_link_options(lldbghidra PRIVATE
    "-F${LLDB_FRAMEWORK_DIR}"
    "-framework" "LLDB"
    "-Wl,-rpath,${LLDB_FRAMEWORK_DIR}"
)

set_target_properties(lldbghidra PROPERTIES
    OUTPUT_NAME "lldbghidra"
    PREFIX ""
    SUFFIX ".dylib"
)
