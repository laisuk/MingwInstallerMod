#ifndef MINGWINSTALLERMOD_SHORTCUT_HPP
#define MINGWINSTALLERMOD_SHORTCUT_HPP
#pragma once

#include <string>
#include <cstddef>

void shortcut(const std::string& filePath,
              const std::string& picturePath,
              const std::string& name,
              std::size_t destination);

std::size_t directory_delete(const std::string& pathname);

void showError(const char* error);
#endif //MINGWINSTALLERMOD_SHORTCUT_HPP