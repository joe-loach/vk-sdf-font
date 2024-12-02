#pragma once

#include "geometry.hpp"
#include "sdf.hpp"
#include "ttfparser.h"

#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdio.h>
#include <string>
#include <vector>

struct Glyph {
  public:
    float width;
    std::vector<SDF> sdfs;
};

struct Font {
  private:
    ttfp_face *_font_face;
    std::string _name;
    uint16_t _units_per_em;
    std::vector<Glyph> _glyphs;

  public:
    static auto from_bytes(const std::vector<std::byte> &font_data) noexcept
        -> std::optional<Font>;

    auto name() -> const std::string & { return _name; }

    auto glyph(const char32_t codepoint) -> std::optional<Glyph> {
        const auto glyph_id = ttfp_get_glyph_index(_font_face, codepoint);

        if (glyph_id == 0)
            return {};

        return _glyphs.at(glyph_id);
    }

  protected:
    Font(ttfp_face *font_face, std::string name, uint16_t units_per_em,
         std::vector<Glyph> glyphs)
        : _font_face(font_face), _name(name), _units_per_em(units_per_em),
          _glyphs(glyphs) {}
};

static auto
get_font_name(const ttfp_face *font_face) -> std::optional<std::string>;

auto Font::from_bytes(const std::vector<std::byte> &font_data) noexcept
    -> std::optional<Font> {
    ttfp_face *font_face = (ttfp_face *)alloca(ttfp_face_size_of());
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
    auto name = get_font_name(font_face).value_or("unknown font");

    // outline all glyphs
    const auto glyph_count = ttfp_get_number_of_glyphs(font_face);

    auto glyphs = std::vector<Glyph>(glyph_count);

    const auto units_per_em = ttfp_get_units_per_em(font_face);

    for (uint32_t glyph_index = 0; glyph_index < glyph_count; ++glyph_index) {

        ttfp_rect bounding_box{};
        auto geom = Geometry{};
        auto outliner = Geometry::outliner();

        ttfp_outline_glyph(font_face, outliner, &geom, glyph_index,
                           &bounding_box);

        auto width = ttfp_get_glyph_hor_advance(font_face, glyph_index);

        Glyph glyph{};
        glyph.sdfs = geom.sdfs;
        glyph.width = width;

        glyphs.push_back(glyph);
    }

    return Font(font_face, name, units_per_em, glyphs);
}

static auto
get_font_name(const ttfp_face *font_face) -> std::optional<std::string> {
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
            delete[] encoded_name;

            return name;
        }
    }

    return {};
}