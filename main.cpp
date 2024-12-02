#include "font.hpp"
#include "io_helper.hpp"

#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <vector>

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << "Too few arguments. Expected `path` and `text`."
                  << std::endl;
        return -1;
    }
    if (argc > 3) {
        std::cerr << "Too many arguments. Expected `path` and `text`."
                  << std::endl;
        return -1;
    }

    auto font_path = argv[1];
    auto text = argv[2];

    std::vector<std::unique_ptr<Line>> lines;
    lines.push_back(std::make_unique<BigLine>());

    const auto bytes = read_file_bytes(font_path);

    if (!bytes) {
        std::cerr << "error reading file";
        return -1;
    }

    auto font = Font::from_bytes(bytes.value());

    if (!font) {
        std::cerr << "failed to read font";
        return -1;
    }

    return 0;
}
