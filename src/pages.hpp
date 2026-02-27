#ifndef MINGWINSTALLER_PAGES_HPP
#define MINGWINSTALLER_PAGES_HPP

#include <cstddef>
#include <cstdint>


class Fl_Widget;

constexpr std::size_t width = 650 + 300;
constexpr std::size_t height = 300 + 10;
constexpr std::size_t png_size = 300;
constexpr std::size_t button_width = 100;
constexpr std::size_t button_height = 25;
constexpr std::size_t button_x = 540;
constexpr std::size_t button_y = 280;

void next_cb(Fl_Widget*, void*);
void back_cb(Fl_Widget*, void*);
void done_cb(Fl_Widget*, void*);

enum class ExcRs { error = 0, sjlj, dwarf, seh };
enum class MgRs  { error = 0, win32, posix, mcf };
enum class Arcs  { error = 0, i686, x86_64 };
enum class Crt   { empty = 0, msvcrt, ucrt, error }; // (see note below)

void showError(const char* error);

#endif //MINGWINSTALLER_PAGES_HPP
