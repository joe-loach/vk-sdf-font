#pragma once

#include <expected>
#include <filesystem>
#include <vector>
#include <fstream>

enum class io_error {
    /// @brief A IO system error
    system,
    /// @brief File doesn't exist
    not_found,
    /// @brief Failed to open file
    open,
    /// @brief Failed to read file
    read,
    /// @brief Failed to close file
    close,
};

auto read_file_bytes(const std::string &path) noexcept
    -> std::expected<std::vector<std::byte>, io_error> {
    std::filesystem::path file_path{path};

    std::error_code err{};

    if (!std::filesystem::exists(file_path, err)) {
        return std::unexpected(io_error::not_found);
    }

    auto length = std::filesystem::file_size(file_path, err);
    if (err) {
        return std::unexpected(io_error::system);
    }
    if (length == 0) {
        // empty vector
        return {};
    }

    std::vector<std::byte> buffer(length);
    std::ifstream input_file(file_path,
                             std::ios_base::in | std::ios_base::binary);
    input_file.unsetf(std::ios::skipws);

    if (!input_file.is_open()) {
        return std::unexpected(io_error::open);
    }

    input_file.read(reinterpret_cast<char *>(buffer.data()), length);

    if (input_file.bad()) {
        return std::unexpected(io_error::read);
    }

    input_file.close();

    if (input_file.fail()) {
        return std::unexpected(io_error::close);
    }

    return buffer;
}