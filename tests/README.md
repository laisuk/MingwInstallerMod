# Safe installation regression tests

Build with `BUILD_TESTING=ON` (the CTest default), then run:

```
cmake --build cmake-build-release
ctest --test-dir cmake-build-release --output-on-failure
```

Python 3 and the 7-Zip command-line program are needed only to generate test
fixtures; neither is a new installer runtime dependency. CMake discovers both.
Tests create unique temporary directories inside the build directory and tiny
synthetic archives. They never discover or modify a real compiler installation.

The native harness calls the production transaction and bundled decoder. It covers:

- Clean installation for both architecture roots and replacement of an existing installation.
- Truncated archive, cancellation during extraction and just before replacement, and a valid archive
  missing gcc.exe.
- A real Windows directory handle without delete sharing, blocking target rename.
- A file blocking creation of an extracted directory.
- Injected publication failure, successful rollback, and failed rollback with the
  old installation retained in the reported backup directory.
- A locked staging file blocking cleanup after successful publication; cleanup
  produces a warning and the new installation remains intact.
- Preservation of unrelated files and directories in every scenario.

Failure tests compare the old compiler sentinel bytes, either at the original
location after rollback or in the retained backup after rollback itself fails.
The sentinel is deliberately not an actual compiler. Tests also check staging
cleanup and new compiler sentinel contents after success.

The transaction extracts under `.mingw-install-<random>/payload` on the destination
filesystem. It validates the selected mingw32/mingw64 root, nonempty bin/gcc.exe,
lib and include directories, and rejects unexpected roots and symbolic links.
Archive path checks prevent absolute paths, parent components, Windows alternate
streams, and trailing-dot/space aliases from escaping staging. Only the selected
compiler directory is moved to `previous` within the workspace. Publication is a
same-filesystem rename. A publication error restores `previous`; a rollback error
retains the entire workspace and reports the recovery path. Backup cleanup starts
only after publication succeeds. Cleanup failure is reported without removing the
published compiler. Shortcuts are generated only after successful publication.

The decoder now checks directory creation, closes output files after write errors,
and routes cancellation through its normal buffer/archive cleanup.
SHFileOperation cancellation is reported by the legacy deletion helper; the
installation transaction no longer uses that helper.

Limitations: two directory renames are not a crash-atomic transaction. Power loss
or process termination between them can require manually restoring `previous`.
Rollback blocked by another process also requires manual recovery. Cancellation is
checked between archive entries and before replacement; decoding one large solid
block cannot be interrupted by this legacy engine. Actual disk exhaustion and
ACL-denial environments are not provisioned by these tests; directory/write/close
errors fail extraction before replacement. Validation checks structure and archive
CRC, not whether gcc can compile a program or archive authenticity. A temporary
copy needs enough free space alongside the existing installation.
