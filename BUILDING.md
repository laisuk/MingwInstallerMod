# Windows x64 builds

CMake 3.23+, Ninja, Git and an x64 MSVC or MinGW compiler are required.
FLTK 1.4.5 is fetched on first configure and built with the active compiler.
Curl and zlib are prebuilt in `vcpkg/windows-msvc` and `vcpkg/windows-mingw`;
no vcpkg checkout, package installation, environment variables or toolchain file
is needed. CMake chooses the bundle from the detected compiler.

For MinGW, put the intended compiler and Ninja on PATH:

```powershell
cmake --preset release-mingw
cmake --build --preset release-mingw
```

For MSVC, use an x64 Native Tools Command Prompt with Ninja on PATH:

```powershell
cmake --preset release-msvc
cmake --build --preset release-msvc
```

Use separate build directories for each compiler. For existing CLion profiles,
remove the old vcpkg toolchain/triplet options and reset the CMake cache before
reloading. Existing caches can retain external curl/zlib locations.

The MSVC bundle uses /MT (Debug: /MTd), including the application and FLTK.
The MinGW bundle's supplied ABI is x86_64-win32-seh-msvcrt, verified in its
source project with GCC 14.2.0. Use a compatible compiler; the triplet name
alone does not guarantee runtime/thread-model compatibility. See each bundle's
NOTICE.md, upstream copyright files, metadata and SHA256SUMS.txt for provenance.
Both Release and Debug libraries are included. Updating curl requires replacing
its matching binary snapshot and zlib dependency, then rebuilding and verifying.

FLTK uses its bundled PNG/JPEG/zlib codecs and only fltk::images is linked.
OpenGL and Forms are disabled. No installed FLTK headers are bundled here.
Windows COM/shell libraries are linked for shortcut creation. MinGW retains
-static and MSVC uses the Windows GUI subsystem.

There is no registered project test suite. Build checks do not exercise installer
downloads, extraction, shortcuts or visual HiDPI behavior; use an interactive
smoke test to verify those behaviors.

## Local validation (2026-11-01)

Fresh Release configure and full builds passed for x64 MinGW GCC 16.2.0
and MSVC 19.44 using the bundled curl/zlib libraries. Both executables
passed the `-v` smoke check (1.2.1-mod1). The existing FLTK 1.4.5 source
checkout was reused for validation; its build output was separate for each
compiler. Debug libraries are included but Debug builds were not revalidated.
