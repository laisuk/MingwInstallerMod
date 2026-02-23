cmake_minimum_required(VERSION 3.23)

# -----------------------------
# vcpkg (single owner)
# -----------------------------
set(CMAKE_TOOLCHAIN_FILE "D:/lib/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "" FORCE)

set(VCPKG_TARGET_TRIPLET "x64-mingw-static"
        CACHE STRING "" FORCE)

project(MingwInstallerMod LANGUAGES C CXX)

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
)

# -----------------------------
# Dependencies
# -----------------------------
find_package(fltk CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)

# FLTK: link by library names (vcpkg MinGW static installs libfltk*.a)
target_link_libraries(${PROJECT_NAME} PRIVATE
        fltk_images
        fltk_forms
        fltk_gl
        fltk
        opengl32
        CURL::libcurl
)

# -----------------------------
# Windows (MinGW) flags / system libs
# -----------------------------
if (WIN32)
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
endif()