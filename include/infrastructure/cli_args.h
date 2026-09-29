#pragma once
#include <cstddef>
#include <string>

namespace vmsim::infrastructure {

enum class PolicyKind { Fifo, Lru };

struct Config {
    std::string inputFile;
    PolicyKind policy = PolicyKind::Fifo;
    size_t physMemBytes = 256 * 1024; // 256KB por defecto (minimo pedido por el enunciado)
    bool verbose = false;
};

// Adaptador para "el mundo exterior es la linea de comandos".
class CliArgs {
public:
    // Lanza std::runtime_error si los argumentos son invalidos.
    static Config parse(int argc, char** argv);
};

} // namespace vmsim::infrastructure
