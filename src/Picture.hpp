#ifndef MINGWINSTALLER_PICTURE_HPP
#define MINGWINSTALLER_PICTURE_HPP

#include <cstddef>
#include <cstdint>

struct Picture{
    std::uint8_t *pointer;
    std::size_t size;
};

#endif //MINGWINSTALLER_PICTURE_HPP
