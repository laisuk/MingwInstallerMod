"""Create small synthetic 7z fixtures and run the native regression harness."""
import pathlib, subprocess, sys, tempfile
with tempfile.TemporaryDirectory(prefix='mingw-archives-', dir=pathlib.Path.cwd()) as tmp:
    root = pathlib.Path(tmp)
    source = root / 'source'
    for d in ('bin', 'lib', 'include'):
        (source / 'mingw64' / d).mkdir(parents=True, exist_ok=True)
    (source / 'mingw64/bin/gcc.exe').write_bytes(b'new compiler')
    (source / 'mingw64/lib/libgcc.a').write_bytes(b'synthetic library')
    subprocess.run([sys.argv[1], 'a', str(root / 'good.7z'), 'mingw64'], cwd=source, check=True, stdout=subprocess.DEVNULL)
    (source / 'mingw64').rename(source / 'mingw32')
    subprocess.run([sys.argv[1], 'a', str(root / 'good32.7z'), 'mingw32'], cwd=source, check=True, stdout=subprocess.DEVNULL)
    (source / 'mingw32').rename(source / 'mingw64')
    (source / 'mingw64/bin/gcc.exe').unlink()
    subprocess.run([sys.argv[1], 'a', str(root / 'partial.7z'), 'mingw64'], cwd=source, check=True, stdout=subprocess.DEVNULL)
    (root / 'corrupt.7z').write_bytes((root / 'good.7z').read_bytes()[:40])
    subprocess.run([sys.argv[2], str(root)], check=True)
