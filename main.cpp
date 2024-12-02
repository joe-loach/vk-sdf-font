#include "font.hpp"
#include "io.hpp"
#include "vulkan.hpp"

#include <stdio.h>

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

    const auto bytes = io::read_file_bytes(font_path);

    if (!bytes) {
        std::cerr << "error reading file";
        return -1;
    }

    auto font = Font::from_bytes(bytes.value());

    if (!font) {
        std::cerr << "failed to read font";
        return -1;
    }

    std::cout << "Rendering '" << text << "' in font: " << font.value().name()
              << std::endl;

    init_vulkan();

    return 0;
}
