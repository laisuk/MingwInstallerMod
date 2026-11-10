#include "InstallTransaction.hpp"
#include <windows.h>
#include <fstream>
#include <iostream>
extern "C" int seven_z(int (*)(size_t, size_t), int, const char **);
namespace fs = std::filesystem;
static bool stop = false;
static int progress(size_t n, size_t) { return stop && n >= 2; }
static std::string read(const fs::path &p) { std::ifstream f(p); return std::string(std::istreambuf_iterator<char>(f), {}); }
static void require(bool ok, const char *message) { if (!ok) throw std::runtime_error(message); }
int main(int argc, char **argv) {
    if (argc != 2) return 2;
    const auto fixtures = fs::absolute(argv[1]);
    const auto base = fixtures / ("mingw-regression-" + std::to_string(GetCurrentProcessId()));
    require(fs::create_directory(base), "test directory already exists");
    int count = 0;
    try {
        for (const std::string scenario : {"clean", "clean32", "finalcancel", "reinstall", "corrupt", "cancel", "locked", "partial", "publish", "rollback", "mkdir", "cleanup"}) {
            const auto parent = base / scenario;
            fs::create_directories(parent);
            std::ofstream(parent / "unrelated.txt") << "keep";
            fs::create_directory(parent / "opt");
            std::ofstream(parent / "opt" / "keep") << "keep";
            const std::string folder = scenario == "clean32" ? "mingw32" : "mingw64";
            const auto target = parent / folder;
            if (scenario != "clean" && scenario != "clean32") {
                fs::create_directories(target / "bin");
                std::ofstream(target / "bin" / "gcc.exe") << "old working compiler";
            }
            HANDLE lock = INVALID_HANDLE_VALUE;
            if (scenario == "locked") {
                lock = CreateFileW(target.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                    nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
                require(lock != INVALID_HANDLE_VALUE, "cannot lock target");
            }
            stop = scenario == "cancel";
            bool failed = false;
            int renames = 0;
            try {
                const auto warning = installation::install(parent, folder, [&](const fs::path &stage) {
                    const auto previous = fs::current_path();
                    fs::current_path(stage);
                    if (scenario == "mkdir") { fs::create_directory("mingw64"); std::ofstream("mingw64/bin") << "block"; }
                    const auto archive = (fixtures / (scenario == "clean32" ? "good32.7z" : scenario == "partial" ? "partial.7z" : scenario == "corrupt" ? "corrupt.7z" : "good.7z")).string();
                    const char *args[] = {"test", "x", archive.c_str()};
                    const auto result = seven_z(progress, 3, args);
                    fs::current_path(previous);
                    return result == 0;
                }, [&] { return scenario == "finalcancel"; }, [&](const fs::path &a, const fs::path &b) {
                    ++renames;
                    if ((scenario == "publish" || scenario == "rollback") && renames == 2)
                        throw std::runtime_error("simulated publication denied");
                    if (scenario == "rollback" && renames == 3)
                        throw std::runtime_error("simulated rollback denied");
                    fs::rename(a, b);
                    if (scenario == "cleanup" && renames == 2) {
                        std::ofstream(a.parent_path() / "locked.tmp") << "retain";
                        lock = CreateFileW((a.parent_path() / "locked.tmp").c_str(), GENERIC_READ,
                            FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
                        require(lock != INVALID_HANDLE_VALUE, "cannot lock staging file");
                    }
                });
                if (scenario == "cleanup") require(!warning.empty(), "cleanup failure was not reported");
            } catch (const std::exception &) { failed = true; }
            if (lock != INVALID_HANDLE_VALUE) CloseHandle(lock);
            const bool success = scenario == "clean" || scenario == "clean32" || scenario == "reinstall" || scenario == "cleanup";
            require(failed != success, "unexpected installation result");
            if (success) require(read(target / "bin/gcc.exe") == "new compiler", "new compiler missing");
            else if (scenario == "rollback") {
                bool recovered = false;
                for (const auto &entry : fs::directory_iterator(parent))
                    if (fs::is_directory(entry) && entry.path().filename().string().find(".mingw-install-") == 0)
                        recovered = read(entry.path() / "previous/bin/gcc.exe") == "old working compiler";
                require(recovered, "backup lost after rollback failure");
            } else require(read(target / "bin/gcc.exe") == "old working compiler", "previous compiler damaged");
            require(read(parent / "unrelated.txt") == "keep" && read(parent / "opt/keep") == "keep", "unrelated files damaged");
            if (scenario != "rollback" && scenario != "cleanup")
                for (const auto &entry : fs::directory_iterator(parent))
                    require(entry.path().filename().string().find(".mingw-install-") != 0, "staging leaked");
            ++count;
        }
        fs::remove_all(base);
        std::cout << count << " installation scenarios passed\n";
    } catch (const std::exception &e) { std::cerr << e.what() << "\nArtifacts: " << base << '\n'; return 1; }
}
