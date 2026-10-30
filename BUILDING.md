# Windows x64 builds

FLTK 1.4.5 is fetched from the `release-1.4.5` tag. Git and network access
are required on the first configure. CMake 3.23+, Ninja, and the selected
compiler must be available. Set `VCPKG_WDIR` (or `VCPKG_ROOT`) to your vcpkg
checkout, or pass `CMAKE_TOOLCHAIN_FILE` explicitly. Note that Visual Studio
may set VCPKG_ROOT to its own bundled checkout; use VCPKG_WDIR or an explicit
toolchain path if your packages are installed elsewhere. The legacy
`D:/lib/vcpkg` fallback is retained for existing local CLion profiles.

Install CURL and its transitive dependencies for each toolchain:

```powershell
& "$env:VCPKG_ROOT/vcpkg.exe" install "curl[core,non-http,sspi,ssl]:x64-mingw-static"
& "$env:VCPKG_ROOT/vcpkg.exe" install "curl[core,non-http,sspi,ssl]:x64-windows-static"
```

Do not install FLTK for this project; an existing vcpkg FLTK installation
can remain installed. CURL is resolved with its vcpkg CONFIG package.

For MinGW, put the intended x64 compiler's bin directory and Ninja on PATH:

```powershell
cmake --preset release-mingw
cmake --build --preset release-mingw
```

For MSVC, run from a **x64 Native Tools Command Prompt** with Ninja on PATH:

```powershell
cmake --preset release-msvc
cmake --build --preset release-msvc
```

The presets use separate `build/release-mingw` and `build/release-msvc`
directories. Never reuse a directory across compilers or vcpkg triplets.
For CLion, use these presets or retain separate Release / Release-Msvc
profiles with their respective toolchains and triplets. After changing this
configuration, use Reset Cache and Reload Project for each existing profile.
For existing local CLion profiles using the dependency installation in
`D:/lib/vcpkg`, set CMake options explicitly:

- Debug / Release: `-DVCPKG_TARGET_TRIPLET=x64-mingw-static`
- Release-Msvc: `-DVCPKG_TARGET_TRIPLET=x64-windows-static`
- All three: `-DCMAKE_TOOLCHAIN_FILE=D:/lib/vcpkg/scripts/buildsystems/vcpkg.cmake`

Reset the CMake cache after changing the toolchain file. Editing only its
cached value can leave CMakeSystem.cmake referencing the previous toolchain.
Compiler-specific flags and CRT settings are handled in CMakeLists.txt after
project(); vcpkg root/triplet selection must happen before project(), when
MSVC/MINGW detection is not yet reliable. Profiles/presets supply that input.

Do not point FETCHCONTENT_BASE_DIR or a dependency binary directory at a
shared directory across toolchains.

The default MSVC triplet, `x64-windows-static`, specifies both static
libraries and a static CRT, so the application, FLTK and its bundled image
codecs use /MT (Debug: /MTd). `x64-windows-static-md` is also supported when
explicitly selected, using /MD (Debug: /MDd) throughout; it requires matching
vcpkg packages and the Visual C++ runtime. Static libraries alone do not
imply static CRT linkage. MinGW retains its existing -static link policy;
its vcpkg triplet specifies static libraries with dynamic CRT linkage.

## Dependency isolation

The old FLTK OpenGL target added the directory containing vcpkg GL/glu.h
as a regular include directory before the fetched FLTK core's transitive
include directories. Because that directory also contains FL/, MSVC could
compile FLTK 1.4.5 sources against the installed FLTK headers.

The application uses widgets and Fl_PNG_Image, but no OpenGL or Forms APIs.
Disabling FLTK_BUILD_GL and FLTK_BUILD_FORMS removes the conflicting GLU
dependency at its source. Only fltk::images is linked, which propagates the
fetched core target. Image codecs remain bundled as in the successful
MinGW build, preventing system codec include roots from reintroducing the
same header conflict. CURL remains a separate application dependency. Some FLTK compile commands
still contain the vcpkg include root, but it follows FLTK's matching build
and source include roots; recorded compiler dependencies confirm that no
vcpkg FL/ headers are used.
No global include ordering override or upstream FLTK source patch is used.
C++17, resources, GUI entry point, and FLTK's default Windows HiDPI backend
are preserved. Windows COM/shell libraries used by shortcut.cpp are explicit.

No project test suite is currently registered. Build validation does not
exercise network downloads, extraction, shortcut creation or visual HiDPI
behavior; those require an interactive installer smoke test.

## Local validation (2026-10-31)

- MinGW-w64 GCC 16.2.0 and MSVC 19.44 x64: clean Release configure and
  complete executable builds succeeded in separate build/verify-* directories.
- Both caches report FLTK 1.4.5, revision a9b1113516ffd15fc7602a6d425a317df30f4720.
- Ninja compiler dependency records contain 4,774 FL/ header references per
  build, all from the fetched source or that compiler's generated FLTK headers;
  none resolve to vcpkg FLTK. Link commands contain one fetched core and one
  fetched Images library, with no installed FLTK, Forms or OpenGL libraries.
- MSVC application and fetched sources use /MT. The installed libcurl archive
  specifies DEFAULTLIB:LIBCMT. No CRT mismatch or linker warnings were reported.
- PE imports contain only Windows system DLLs; neither executable requires
  FLTK/CURL DLLs or compiler runtime redistributable DLLs. Both use Windows GUI
  subsystem and pass the `-v` smoke test (exit 0, version 1.2.1-mod1).
- CTest ran for both build directories: no tests were found.
- MSVC Debug try-compile checks initially encountered local executable locks
  and a stalled PDB server. Rechecking with
  `-DCMAKE_TRY_COMPILE_CONFIGURATION=Release` succeeded. This is a validation
  workaround for the local environment, not a project-wide compiler flag.
- The optional static-md triplet and interactive installer/HiDPI behavior have
  not been tested. Existing installer source code was not changed.
