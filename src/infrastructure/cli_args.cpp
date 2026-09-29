#include "infrastructure/cli_args.h"
#include <charconv>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include "domain/types.h"

namespace vmsim::infrastructure {

namespace {

size_t parseSizeWithSuffix(const std::string& text) {
    if (text.empty()) {
        throw std::runtime_error("Valor de tamano vacio");
    }
    size_t multiplier = 1;
    std::string numberPart = text;
    char last = text.back();
    if (last == 'K' || last == 'k') {
        multiplier = 1024;
        numberPart = text.substr(0, text.size() - 1);
    } else if (last == 'M' || last == 'm') {
        multiplier = 1024 * 1024;
        numberPart = text.substr(0, text.size() - 1);
    }
    uint64_t number = 0;
    auto result = std::from_chars(numberPart.data(), numberPart.data() + numberPart.size(), number);
    if (result.ec != std::errc{} || result.ptr != numberPart.data() + numberPart.size()) {
        throw std::runtime_error("Tamano invalido: " + text);
    }
    if (number > std::numeric_limits<uint64_t>::max() / multiplier) {
        throw std::runtime_error("Tamano fuera de rango: " + text);
    }
    uint64_t bytes = number * multiplier;
    if (bytes > std::numeric_limits<size_t>::max()) {
        throw std::runtime_error("Tamano fuera de rango: " + text);
    }
    return static_cast<size_t>(bytes);
}

std::string valueOf(const std::string& arg) {
    auto pos = arg.find('=');
    if (pos == std::string::npos) {
        throw std::runtime_error("Argumento invalido (esperado --flag=valor): " + arg);
    }
    return arg.substr(pos + 1);
}

} // namespace

Config CliArgs::parse(int argc, char** argv) {
    Config config;
    bool inputSet = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.rfind("--policy=", 0) == 0) {
            std::string v = valueOf(arg);
            if (v == "fifo") config.policy = PolicyKind::Fifo;
            else if (v == "lru") config.policy = PolicyKind::Lru;
            else throw std::runtime_error("Politica desconocida: " + v + " (use fifo o lru)");
        } else if (arg.rfind("--phys-mem=", 0) == 0) {
            config.physMemBytes = parseSizeWithSuffix(valueOf(arg));
        } else if (arg == "-v" || arg == "--verbose") {
            config.verbose = true;
        } else if (!inputSet) {
            config.inputFile = arg;
            inputSet = true;
        } else {
            throw std::runtime_error("Argumento no reconocido: " + arg);
        }
    }

    if (!inputSet) {
        throw std::runtime_error(
            "Uso: vmsim <archivo_comandos> [--policy=fifo|lru] [--phys-mem=256K] [-v]");
    }
    constexpr size_t minPhysicalMemory = 256 * 1024;
    if (config.physMemBytes < minPhysicalMemory) {
        throw std::runtime_error("--phys-mem debe ser de al menos 256K");
    }
    if (config.physMemBytes % domain::PAGE_SIZE != 0) {
        throw std::runtime_error("--phys-mem debe ser multiplo de 4096 bytes");
    }

    return config;
}

} // namespace vmsim::infrastructure
