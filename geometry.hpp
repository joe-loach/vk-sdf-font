#pragma once

#include "math.hpp"
#include "sdf.hpp"
#include "ttfparser.h"
#include <cstdint>
#include <stdio.h>
#include <vector>
#include <memory>

struct Geometry {
private:
    glm::vec2 start;
    glm::vec2 prev;
public:
    std::vector<SDF> sdfs;

public:
    static ttfp_outline_builder outliner() {
        ttfp_outline_builder builder;
        builder.move_to = Geometry::move_to;
        builder.line_to = Geometry::line;
        builder.quad_to = Geometry::quad;
        builder.curve_to = Geometry::curve;
        builder.close_path = Geometry::close_path;
        return builder;
    };

private:
    static void move_to(float x, float y, void *data) {
        auto geom = static_cast<Geometry *>(data);
        geom->start = glm::vec2(x, y);
        geom->prev = glm::vec2(x, y);
    }

    static void line(float x, float y, void *data) {
        auto geom = static_cast<Geometry *>(data);

        auto start = geom->prev;
        auto end = glm::vec2(x, y);

        geom->sdfs.push_back(SDF{Line(start, end)});
        geom->prev = end;
    }

    static void quad(float cx, float cy, float x, float y, void *data) {
        auto geom = static_cast<Geometry *>(data);

        auto start = geom->prev;
        auto end = glm::vec2(x, y);
        auto control = glm::vec2(cx, cy);

        geom->sdfs.push_back(SDF{Quad(start, end, control)});
        geom->prev = end;
    }

    static void curve(float cax, float cay, float cbx, float cby, float x, float y,
                void *data) {
        auto geom = static_cast<Geometry *>(data);

        auto start = geom->prev;
        auto end = glm::vec2(x, y);
        auto control_a = glm::vec2(cax, cay);
        auto control_b = glm::vec2(cbx, cby);

        geom->sdfs.push_back(SDF{Curve(start, end, control_a, control_b)});
        geom->prev = end;
    }

    static void close_path(void *data) {
        auto geom = static_cast<Geometry *>(data);

        if (geom->start != geom->prev) {
            geom->sdfs.push_back(SDF{Line(geom->prev, geom->start)});
        }
        geom->prev = geom->start;
    }
};
