# Bundled MSVC dependency notice

These prebuilt x64 Windows libraries were copied from
`D:/lib/vcpkg/installed/x64-windows-static`. They use the MSVC ABI and the
**static Microsoft C/C++ runtime**: `/MT` for Release and `/MTd` for Debug.
They are separate from the MinGW `x86_64-win32-seh-msvcrt` binaries.

The SDK adapter sets `CMAKE_MSVC_RUNTIME_LIBRARY` to match these libraries.
Local builds are verified with Visual Studio 2022 MSVC 19.44.35229.0 (v143).
The retained installed metadata records package ABI hashes, not the exact
original compiler version. Use a compatible x64 MSVC toolset and Windows SDK.

## Included packages

Only curl 8.14.1 and zlib 1.3.1 are retained here, including their headers,
Release/Debug static libraries, CMake packages and upstream license notices.
License files: installed/x64-windows-static/share/{curl,zlib}/copyright.
Source: D:/codes/CPP/MingwDownloader/vcpkg/windows-msvc.
The metadata directory retains the original package file lists (some listed
files may have already been omitted by the source snapshot).
SHA256SUMS.txt records this copy's payload. No package manager is required.
To update, replace both packages with matching ABI builds and refresh hashes.
