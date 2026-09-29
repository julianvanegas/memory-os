#include "infrastructure/command_file_parser.h"
#include <charconv>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace vmsim::infrastructure {

using application::Command;
using application::CommandType;

namespace {
bool parseUint32(const std::string& text, uint32_t& value) {
    if (text.empty()) return false;

    uint64_t parsed = 0;
    auto result = std::from_chars(text.data(), text.data() + text.size(), parsed);
    if (result.ec != std::errc{} || result.ptr != text.data() + text.size() ||
        parsed > std::numeric_limits<uint32_t>::max()) {
        return false;
    }
    value = static_cast<uint32_t>(parsed);
    return true;
}

bool hasNoExtraTokens(std::istringstream& iss) {
    std::string extra;
    return !(iss >> extra);
}
} // namespace

std::vector<Command> CommandFileParser::parse(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo de entrada: " + path);
    }

    std::vector<Command> commands;
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;
        std::istringstream iss(line);
        std::string verb;
        if (!(iss >> verb)) continue;   // linea vacia
        if (verb[0] == '#') continue;   // comentario

        Command cmd;
        cmd.lineNumber = lineNumber;
        bool ok = true;

        if (verb == "alloc") {
            cmd.type = CommandType::Alloc;
            std::string bytes;
            ok = static_cast<bool>(iss >> bytes) && parseUint32(bytes, cmd.bytes) &&
                 hasNoExtraTokens(iss);
            if (!ok) {
                std::cerr << "Error de parseo (linea " << lineNumber << "): 'alloc' requiere <bytes>\n";
            }
        } else if (verb == "write") {
            cmd.type = CommandType::Write;
            std::string address;
            std::string value;
            uint32_t parsedValue = 0;
            ok = static_cast<bool>(iss >> address >> value) &&
                 parseUint32(address, cmd.address) && parseUint32(value, parsedValue) &&
                 parsedValue <= std::numeric_limits<uint8_t>::max() &&
                 hasNoExtraTokens(iss);
            if (ok) cmd.value = static_cast<uint8_t>(parsedValue);
            if (!ok) {
                std::cerr << "Error de parseo (linea " << lineNumber << "): 'write' requiere <addr> <value>\n";
            }
        } else if (verb == "read") {
            cmd.type = CommandType::Read;
            std::string address;
            ok = static_cast<bool>(iss >> address) && parseUint32(address, cmd.address) &&
                 hasNoExtraTokens(iss);
            if (!ok) {
                std::cerr << "Error de parseo (linea " << lineNumber << "): 'read' requiere <addr>\n";
            }
        } else if (verb == "free") {
            cmd.type = CommandType::Free;
            std::string address;
            ok = static_cast<bool>(iss >> address) && parseUint32(address, cmd.address) &&
                 hasNoExtraTokens(iss);
            if (!ok) {
                std::cerr << "Error de parseo (linea " << lineNumber << "): 'free' requiere <addr>\n";
            }
        } else {
            std::cerr << "Error de parseo (linea " << lineNumber << "): verbo desconocido '" << verb << "'\n";
            ok = false;
        }

        if (ok) {
            commands.push_back(cmd);
        }
    }

    return commands;
}

} // namespace vmsim::infrastructure
