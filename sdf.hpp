#pragma once

#include "math.hpp"
#include <variant>

class Line {
  private:
    glm::vec2 start;
    glm::vec2 end;

  public:
    Line(glm::vec2 start, glm::vec2 end) : start(start), end(end) {}
};

class Quad {
  private:
    glm::vec2 start;
    glm::vec2 end;
    glm::vec2 control;

  public:
    Quad(glm::vec2 start, glm::vec2 end, glm::vec2 control)
        : start(start), end(end), control(control) {}
};

class Curve {
  private:
    glm::vec2 start;
    glm::vec2 end;
    glm::vec2 control_a;
    glm::vec2 control_b;

  public:
    Curve(glm::vec2 start, glm::vec2 end, glm::vec2 control_a,
          glm::vec2 control_b)
        : start(start), end(end), control_a(control_a), control_b(control_b) {}
};

using SDF = std::variant<Line, Quad, Curve>;
