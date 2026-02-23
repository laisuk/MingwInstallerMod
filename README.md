<!-- README.md -->

<p align="center">
  <img src="resources/app.ico" width="96" height="96" alt="MingwInstallerMod icon">
</p>

<h1 align="center">MingwInstallerMod</h1>

<p align="center">
  A modified GUI online installer for MinGW-w64 (Windows).
</p>

<p align="center">
  <a href="https://github.com/Vuniverse0/mingwInstaller">Original project</a>
  ·
  <a href="LICENSE">License (Apache-2.0)</a>
  ·
  <a href="https://github.com/Vuniverse0/mingwInstaller/releases">Upstream releases</a>
</p>

---
[![GitHub Release](https://img.shields.io/github/v/release/laisuk/MingwInstallerMod?display_name=tag&sort=semver)](https://github.com/laisuk/MingwInstallerMod/releases/latest)
[![Total Downloads](https://img.shields.io/github/downloads/laisuk/MingwInstallerMod/total.svg)](https://github.com/laisuk/MingwInstallerMod/releases)
[![Latest Downloads](https://img.shields.io/github/downloads/laisuk/MingwInstallerMod/latest/total.svg)](https://github.com/laisuk/MingwInstallerMod/releases/latest)
[![VirusTotal](https://img.shields.io/badge/VirusTotal-0%2F72%20Clean-brightgreen)](https://www.virustotal.com/gui/file/bcc06c50a7da7899bc7f7b4baf5d6a8d705c88b9066405f081f6f46b30c1471c)

## About

**MingwInstallerMod** is a modified version of
[`mingwInstaller`](https://github.com/Vuniverse0/mingwInstaller) by **Vuniverse**.

This fork contains additional modifications and refactoring.  
It is **not** an official upstream release.

## Libraries used

- [FLTK](https://www.fltk.org/)
- [libcurl](https://curl.se/libcurl/)
- 7z sources (part of [LZMA SDK](https://www.7-zip.org/sdk.html))

## Distribution

Icons are embedded in binary and libraries are linked statically,
so the installer runs as a single `.exe` file.

---

## Integrity & Security

The official `MingwInstallerMod.exe` release is scanned on VirusTotal:

| File                  | Result              |
|-----------------------|---------------------|
| MingwInstallerMod.exe | 0 / 72 – Undetected |

View full report:  
[[Virus Total Scan](https://www.virustotal.com/gui/file/bcc06c50a7da7899bc7f7b4baf5d6a8d705c88b9066405f081f6f46b30c1471c)]

---

## License

This project is licensed under the **Apache License 2.0** — see [LICENSE](LICENSE).

Upstream project:

- [`mingwInstaller`](https://github.com/Vuniverse0/mingwInstaller) by **Vuniverse**
