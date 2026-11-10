#pragma once
#include <filesystem>
#include <functional>
#include <random>
#include <stdexcept>
#include <string>

namespace installation {
namespace fs = std::filesystem;
using Rename = std::function<void(const fs::path &, const fs::path &)>;
// Only an exclusively created workspace is ever recursively removed.
inline std::string install(const fs::path &parentPath, const std::string &folder,
                           const std::function<bool(const fs::path &)> &extract,
                           const std::function<bool()> &cancelled,
                           Rename rename = [](const fs::path &a, const fs::path &b) { fs::rename(a, b); }) {
    const auto parent = fs::absolute(parentPath);
    const auto target = parent / folder;
    fs::path work;
    std::random_device random;
    for (int i = 0; i != 100; ++i) {
        auto candidate = parent / (".mingw-install-" + std::to_string(random()) + "-" + std::to_string(random()));
        if (fs::create_directory(candidate)) { work = candidate; break; }
    }
    if (work.empty()) throw std::runtime_error("Cannot create installation staging directory");
    const auto payload = work / "payload";
    const auto backup = work / "previous";
    bool saved = false;
    bool published = false;
    auto cleanup = [&]() {
        std::error_code ec;
        fs::remove_all(work, ec);
        return ec ? "Temporary files retained at " + work.string() + ": " + ec.message() : std::string();
    };
    try {
        fs::create_directory(payload);
        if (!extract(payload) || cancelled())
            throw std::runtime_error("Extraction failed or was cancelled");
        const auto source = payload / folder;
        // Reject links/reparse-style redirections and unexpected top-level contents.
        for (const auto &entry : fs::recursive_directory_iterator(payload)) {
            if (fs::is_symlink(entry.symlink_status()))
                throw std::runtime_error("Extracted symbolic link is not supported");
        }
        for (const auto &entry : fs::directory_iterator(payload))
            if (entry.path().filename() != folder)
                throw std::runtime_error("Unexpected archive root");
        const auto gcc = source / "bin" / "gcc.exe";
        if (!fs::is_directory(source / "bin") || !fs::is_directory(source / "lib") ||
            !fs::is_directory(source / "include") || !fs::is_regular_file(gcc) || fs::file_size(gcc) == 0)
            throw std::runtime_error("Incomplete compiler archive (expected bin/gcc.exe, lib and include)");
        if (cancelled()) throw std::runtime_error("Installation cancelled");
        if (fs::exists(fs::symlink_status(target))) {
            if (fs::is_symlink(fs::symlink_status(target)) || !fs::is_directory(target))
                throw std::runtime_error("Installation target is not a normal directory");
            rename(target, backup);
            saved = true;
        }
        try {
            rename(source, target);
            published = true;
        } catch (...) {
            if (saved) {
                try { rename(backup, target); saved = false; }
                catch (const std::exception &e) {
                    throw std::runtime_error("Replacement and rollback failed. Previous installation preserved at " +
                                             backup.string() + "; restore it manually. " + e.what());
                }
            }
            throw;
        }
        // Publication succeeded. Cleanup errors are warnings, never a reason to delete target.
        return cleanup();
    } catch (const std::exception &e) {
        std::string message = e.what();
        if (!saved && !published) {
            auto warning = cleanup();
            if (!warning.empty()) message += "\n" + warning;
        } else message += "\nRecoverable files retained at " + work.string();
        throw std::runtime_error(message);
    }
}
} // namespace installation
