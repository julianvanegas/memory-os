#pragma once
#include <string>
#include <vector>
#include "application/command.h"

namespace vmsim::infrastructure {

// Adaptador para "el mundo exterior es un archivo de texto". Es el unico
// archivo del proyecto que toca std::ifstream.
class CommandFileParser {
public:
    // Lanza std::runtime_error si el archivo no se puede abrir. Las lineas
    // malformadas se reportan por stderr y se saltan (no abortan el parseo).
    static std::vector<application::Command> parse(const std::string& path);
};

} // namespace vmsim::infrastructure
