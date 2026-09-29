#pragma once
#include <cstdint>

namespace vmsim::application {

enum class CommandType { Alloc, Write, Read, Free };

// DTO deliberadamente tonto: sin parsing ni validacion, solo la forma que
// infrastructure produce y application consume.
struct Command {
    CommandType type;
    uint32_t address = 0;  // usado por write/read/free
    uint32_t bytes = 0;    // usado por alloc
    uint8_t value = 0;     // usado por write
    int lineNumber = 0;    // para mensajes de error legibles
};

} // namespace vmsim::application
