#pragma once

#include "ttfparser.h"

#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <vector>

class Line {
  public:
    virtual float distance(float, float) = 0;

    virtual ~Line() = default;
};

class BigLine: public Line {
  public:
    float distance(float x, float y) override {
        return x + y;
    }
};

struct Glyph {
  private:
    float width;
    std::vector<std::shared_ptr<Line>> lines;
};

struct Font {
  private:
    std::string name;
    std::unordered_map<char32_t, Glyph> glyphs;

  public:
    static auto from_bytes(const std::vector<std::byte> &font_data) noexcept
        -> std::optional<Font> {
        ttfp_face *font_face = (ttfp_face *)malloc(ttfp_face_size_of());
        if (font_face == nullptr) {
            return {};
        }

        auto success =
            ttfp_face_init(reinterpret_cast<const char *>(font_data.data()),
                           font_data.size(), 0, font_face);

        if (!success) {
            return {};
        }

        // find the name of the font

        const auto name_count = ttfp_get_name_records_count(font_face);

        const uint16_t FULL_NAME = 4;
        const uint16_t UNICODE = 0;
        const uint16_t WINDOWS = 3;
        // https://docs.microsoft.com/en-us/typography/opentype/spec/name#windows-encoding-ids
        const uint16_t WINDOWS_SYMBOL_ENCODING_ID = 0;
        const uint16_t WINDOWS_UNICODE_BMP_ENCODING_ID = 1;

        for (auto index = 0; index < name_count; ++index) {
            ttfp_name_record record{};
            ttfp_get_name_record(font_face, index, &record);
            bool is_unicode =
                record.platform_id == UNICODE ||
                (record.platform_id == WINDOWS &&
                 (record.encoding_id == WINDOWS_SYMBOL_ENCODING_ID ||
                  record.encoding_id == WINDOWS_UNICODE_BMP_ENCODING_ID));

            if (record.name_id == FULL_NAME && is_unicode) {
                char *encoded_name = new char[record.name_size];
                bool success = ttfp_get_name_record_string(
                    font_face, index, encoded_name, record.name_size);

                if (!success) {
                    delete[] encoded_name;
                    return {};
                }

                std::string name(encoded_name, encoded_name + record.name_size);

                break;
            }
        }

        // collect all the unique codepoints to the glyph mappings
        // const auto glyph_count = ttfp_get_number_of_glyphs(font_face);

        auto glyphs = std::unordered_map<char32_t, Glyph>{};

        // all printable ascii characters
        for (uint32_t codepoint = 32; codepoint < 127; ++codepoint) {
            auto mapped_index = ttfp_get_glyph_index(font_face, codepoint);

        }

        free(font_face);

        return Font("UNK", glyphs);
    }

  protected:
    Font(std::string name, std::unordered_map<char32_t, Glyph> glyphs)
        : name(name), glyphs(glyphs) {}
};
