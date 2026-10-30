cmake_minimum_required(VERSION 3.23)

# -----------------------------
# vcpkg (single owner)
# -----------------------------
if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if (DEFINED ENV{VCPKG_WDIR})
        set(CMAKE_TOOLCHAIN_FILE
                "$ENV{VCPKG_WDIR}/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "" FORCE)
    elseif (DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE
                "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
                CACHE FILEPATH "vcpkg toolchain")
    else ()
        set(CMAKE_TOOLCHAIN_FILE
                "D:/lib/vcpkg/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "" FORCE)
    endif ()
endif ()

# Explicit triplets from presets/CLion take precedence. With Ninja, specify
# the compiler (or CXX) when selecting MinGW; Windows otherwise defaults to cl.
if (NOT DEFINED VCPKG_TARGET_TRIPLET)
    if (CMAKE_GENERATOR MATCHES "MinGW" OR
            CMAKE_CXX_COMPILER MATCHES "(g[+][+]|c[+][+])" OR
            "$ENV{CXX}" MATCHES "(g[+][+]|c[+][+])")
        set(VCPKG_TARGET_TRIPLET "x64-mingw-static" CACHE STRING "vcpkg target triplet")
    else ()
        set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "vcpkg target triplet")
    endif ()
endif ()

project(MingwInstallerMod LANGUAGES C CXX RC)

if (NOT WIN32 OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "MingwInstallerMod requires a Windows x64 toolchain")
endif ()

# Match the actual CRT linkage of the supported vcpkg triplets. Static
# libraries can use either CRT: x64-windows-static-md deliberately uses /MD.
if (MSVC)
    if (VCPKG_TARGET_TRIPLET STREQUAL "x64-windows-static")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        set(FLTK_MSVC_RUNTIME_DLL OFF CACHE BOOL "Match vcpkg CRT" FORCE)
    elseif (VCPKG_TARGET_TRIPLET STREQUAL "x64-windows-static-md")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        set(FLTK_MSVC_RUNTIME_DLL ON CACHE BOOL "Match vcpkg CRT" FORCE)
    else ()
        message(FATAL_ERROR "MSVC requires x64-windows-static or x64-windows-static-md")
    endif ()
elseif (NOT MINGW OR NOT VCPKG_TARGET_TRIPLET STREQUAL "x64-mingw-static")
    message(FATAL_ERROR "MinGW requires x64-mingw-static; use a separate build directory for each compiler")
endif ()

# -----------------------------
# Language / standard
# -----------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------
# Target
# -----------------------------
add_executable(${PROJECT_NAME}
        src/main.cpp
        src/unpack.c
        src/DownloadButton.cpp
        src/Manager.cpp

        src/page_about.cpp
        src/page0.cpp
        src/page1.cpp
        src/page2.cpp
        src/page3.cpp
        src/page4.cpp
        src/page5.cpp
        src/page6.cpp
        src/page7.cpp
        src/page8.cpp

        src/logo.cpp
        src/icon.cpp
        src/home.cpp
        src/shortcut.cpp

        src/lzma2201/C/Bcj2.c
        src/lzma2201/C/Bra.c
        src/lzma2201/C/Bra86.c
        src/lzma2201/C/BraIA64.c
        src/lzma2201/C/CpuArch.c
        src/lzma2201/C/Delta.c
        src/lzma2201/C/Lzma2Dec.c
        src/lzma2201/C/LzmaDec.c
        src/lzma2201/C/Ppmd7.c
        src/lzma2201/C/Ppmd7Dec.c
        src/lzma2201/C/7zCrc.c
        src/lzma2201/C/7zCrcOpt.c
        src/lzma2201/C/Sha256.c
        src/lzma2201/C/Sha256Opt.c
        src/lzma2201/C/7zAlloc.c
        src/lzma2201/C/7zArcIn.c
        src/lzma2201/C/7zBuf.c
        src/lzma2201/C/7zBuf2.c
        src/lzma2201/C/7zDec.c
        src/lzma2201/C/7zFile.c
        src/lzma2201/C/7zStream.c

        resources/app.rc
        src/shortcut.hpp
)

# -----------------------------
# Dependencies
# -----------------------------
# Build the same FLTK release used by the newer MingwDownloader.
# PNG decoding (Manager::logo) requires Images. No OpenGL or Forms APIs
# are used. Disabling GL also prevents its vcpkg GLU include directory
# from shadowing the fetched FLTK headers (FLTK 1.4.5 include ordering).
include(FetchContent)
set(FLTK_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_FLUID OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_FLTK_OPTIONS OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_GL OFF CACHE BOOL "" FORCE)
set(FLTK_BUILD_FORMS OFF CACHE BOOL "" FORCE)

# Keep image codecs local to FLTK, as in the working MinGW configuration.
# Their include paths cannot introduce another installation's FL/ headers.
set(FLTK_USE_SYSTEM_LIBJPEG OFF CACHE BOOL "" FORCE)
set(FLTK_USE_SYSTEM_LIBPNG OFF CACHE BOOL "" FORCE)
set(FLTK_USE_SYSTEM_ZLIB OFF CACHE BOOL "" FORCE)

FetchContent_Declare(fltk
        GIT_REPOSITORY https://github.com/fltk/fltk.git
        GIT_TAG release-1.4.5
        GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(fltk)

# CURL continues to come from the existing vcpkg installation.
find_package(CURL CONFIG REQUIRED)

# Use only fetched targets; Images propagates the matching core FLTK target.
target_link_libraries(${PROJECT_NAME} PRIVATE
        fltk::images
        CURL::libcurl
)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ole32 shell32 uuid)
endif ()

# -----------------------------
# Windows (MinGW) flags / system libs
# -----------------------------
if (WIN32)
    if (MINGW)
        target_compile_definitions(${PROJECT_NAME} PRIVATE
                NGHTTP2_STATICLIB
                CURL_STATICLIB
                NO_SSL_DL
        )

        target_compile_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Release>:-O2>
                $<$<CONFIG:Release>:-ffunction-sections>
                $<$<CONFIG:Release>:-fdata-sections>
        )

        # Keep these as link options (cleaner than mixing into libraries)
        target_link_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Release>:-Wl,--gc-sections>
                $<$<CONFIG:Release>:-s>
                -mwindows
                -static
        )
    elseif (MSVC)
        target_compile_definitions(${PROJECT_NAME} PRIVATE
                NGHTTP2_STATICLIB
                CURL_STATICLIB
                NO_SSL_DL
        )

        # GUI subsystem (no console)
        target_link_options(${PROJECT_NAME} PRIVATE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)

        # CRT selection above applies to the application and fetched libraries.
    endif ()
endif ()
