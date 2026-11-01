# Bundled MinGW dependency notice

These binaries were copied from `D:/lib/vcpkg/installed/x64-mingw-static`.
Their supplied build origin is **x86_64-win32-seh-msvcrt**: x64 Windows,
win32 thread model, SEH exceptions, and the MSVCRT C runtime. This is distinct
from the vcpkg triplet name `x64-mingw-static`, which does not specify those ABI
choices. Local Release and Debug builds were verified using MinGW-Builds
GCC 14.2.0 (x86_64-win32-seh-rev2, MSVCRT). The original compiler provenance
is supplied by the repository owner; the installed metadata does not independently
record every compiler option. Use an ABI-compatible toolchain.

## Included packages

Only curl 8.14.1 and zlib 1.3.1 are retained here, including their headers,
Release/Debug static libraries, CMake packages and upstream license notices.
License files: installed/x64-mingw-static/share/{curl,zlib}/copyright.
Source: D:/codes/CPP/MingwDownloader/vcpkg/windows-mingw.
The metadata directory retains the original package file lists (some listed
files may have already been omitted by the source snapshot).
SHA256SUMS.txt records this copy's payload. No package manager is required.
To update, replace both packages with matching ABI builds and refresh hashes.
