#include "test_framework.h"
#include "infrastructure/command_file_parser.h"
#include <cstdio>
#include <fstream>

using namespace vmsim::infrastructure;
using namespace vmsim::application;

namespace {
std::string writeTempFile(const char* name, const std::string& contents) {
    std::string path = std::string("/tmp/") + name;
    std::ofstream out(path);
    out << contents;
    out.close();
    return path;
}
} // namespace

TEST(parser_reads_all_command_types) {
    std::string path = writeTempFile("vmsim_test_all_types.txt",
        "alloc 8192\n"
        "write 0 42\n"
        "read 0\n"
        "free 0\n");
    auto commands = CommandFileParser::parse(path);
    ASSERT_EQ(commands.size(), 4u);
    ASSERT_TRUE(commands[0].type == CommandType::Alloc);
    ASSERT_TRUE(commands[1].type == CommandType::Write);
    ASSERT_TRUE(commands[2].type == CommandType::Read);
    ASSERT_TRUE(commands[3].type == CommandType::Free);
    std::remove(path.c_str());
}

TEST(parser_skips_blank_lines_and_comments) {
    std::string path = writeTempFile("vmsim_test_comments.txt",
        "# comentario\n"
        "\n"
        "alloc 4096\n");
    auto commands = CommandFileParser::parse(path);
    ASSERT_EQ(commands.size(), 1u);
    std::remove(path.c_str());
}

TEST(parser_skips_malformed_lines) {
    std::string path = writeTempFile("vmsim_test_malformed.txt",
        "alloc\n"        // falta <bytes>
        "write 0 42\n"); // esta linea si es valida
    auto commands = CommandFileParser::parse(path);
    ASSERT_EQ(commands.size(), 1u);
    std::remove(path.c_str());
}

TEST(parser_rejects_invalid_ranges_and_extra_tokens) {
    std::string path = writeTempFile("vmsim_test_invalid_values.txt",
        "alloc -1\n"
        "write 0 300\n"
        "read 0 extra\n"
        "alloc 4096\n");
    auto commands = CommandFileParser::parse(path);
    ASSERT_EQ(commands.size(), 1u);
    ASSERT_TRUE(commands[0].type == CommandType::Alloc);
    std::remove(path.c_str());
}
