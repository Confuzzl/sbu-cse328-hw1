#include "scene.h"

#include <cmath>

#include "app.h"

static float quadraticXY(const float x, const float y, const float a,
                         const float b, const float c) {
  return (a * x * x) + (b * x) - y + c;
};
static float quadraticF(const float x, const float a, const float b,
                        const float c) {
  return (a * x * x) + (b * x) + c;
}
static glm::ivec2 quadraticXFast(std::vector<glm::vec2> &points,
                                 const glm::ivec2 offset,
                                 const glm::vec3 params,
                                 const glm::ivec2 start) {
  const auto a = params.x, b = params.y, c = params.z;

  const int aSign = a > 0 ? +1 : -1;
  const int stop = (aSign - b) / (2 * a);

  const auto center = start.x;

  glm::ivec2 curr = start;
  auto p = quadraticXY(curr.x + 1, curr.y + aSign * 0.5, a, b, c);
  for (; curr.x < stop; curr.x++) {
    points.emplace_back(Scene::asBig(curr + offset));
    points.emplace_back(
        Scene::asBig(glm::ivec2{2 * center - curr.x, curr.y} + offset));
    const float common = a * (2 * curr.x + 3) + b;
    if (aSign * p < 0) {
      // p(i + 1) = a(x+2)^2 + b(x+2) - (y +- 0.5) + c
      //  = a(x^2+4x+4) + b(x+2) - (y + 0.5) + c
      // p(i) = a(x^2+2x+1) + b(x+1) - (y +- 0.5) + c
      // p(i + 1) - p(i) = a(2x+3) + b

      p += common;
    } else {
      // p(i + 1) = a(x+2)^2 + b(x+2) - (y +- 1.5) + c
      //  = a(x^2+4x+4) + b(x+2) - (y + 1.5) + c
      // p(i) = a(x^2+2x+1) + b(x+1) - (y +- 0.5) + c
      // p(i + 1) - p(i) = a(2x+3) + b -+ 1

      p += common - aSign;
      curr.y += aSign;
    }
  }
  return curr;
}
static void quadraticYFast(std::vector<glm::vec2> &points,
                           const glm::ivec2 offset, const glm::vec3 params,
                           glm::ivec2 start, const int center) {
  const auto a = params.x, b = params.y, c = params.z;
  const auto aSign = a > 0 ? +1 : -1;

  auto p = quadraticXY(start.x + 0.5, start.y, a, b, c);
  for (; -offset.y <= start.y && start.y <= app().framebufferSize.y - offset.y;
       start.y += aSign) {
    points.emplace_back(Scene::asBig(start + offset));
    points.emplace_back(
        Scene::asBig(glm::ivec2{2 * center - start.x, start.y} + offset));
    if (aSign * p < 0) {
      // p(i + 1) = a(x^2 + 3x + 2.25) + b(x + 1.5) - (y +- 2) + c
      // p(i) = a(x^2 + x + 0.25) + b(x + 0.5) - (y +- 1) + c
      // p(i + 1) - p(i) = a(2x + 2) + b +- 1

      p += a * 2 * (start.x + 1) + b - aSign * 1;
      start.x++;
    } else {
      // p(i + 1) = a(x^2 + x + 0.25) + b(x + 0.5) - (y +- 2) + c
      // p(i) = a(x^2 + x + 0.25) + b(x + 0.5) - (y +- 1) + c
      // p(i + 1) - p(i) = +-1

      p -= aSign;
    }
  }
}
void Scene::quadratic(std::vector<glm::vec2> &points, const glm::vec3 params) {
  const auto a = params.x, b = params.y, c = params.z;

  const int h = -b / (2 * a), k = quadraticF(h, a, b, c);
  const glm::ivec2 vertex{h, k};

  //   +- |       | -+
  //   |  |       |  |
  // D |  |       |  | C
  //   |   \     /   |
  //   +-   ~_._~   -+
  //
  //        | | |
  //        +-+-+
  //         B A
  //
  // A: [ ][2] B: [2][ ]
  //    [X][1]    [1][X]
  //
  // C: [1][2] D: [2][1]
  //    [X][ ]    [ ][X]
  //
  // dy/dx = 2ax + b
  // dy/dx = +1; x = +(1-b)/2a
  // dy/dx = -1; x = -(1+b)/2a

  if (a == 0) {
    if (b == 0) {
      for (int x = 0; x < app().framebufferSize.x; x++) {
        points.emplace_back(Scene::asBig({x, c + bonusOffset.y}));
      }
    } else {
    }
  } else {
    // DRAW A AND REFLECT FOR B
    auto cStart = quadraticXFast(points, bonusOffset, params, vertex);
    // DRAW C AND REFLECT FOR D
    quadraticYFast(points, bonusOffset, params, cStart, h);
  }
}

static float cubicXY(const float x, const float y, const float a, const float b,
                     const float c, const float d) {
  return (a * x * x * x) + (b * x * x) + (c * x) - y + d;
};
static float cubicF(const float x, const float a, const float b, const float c,
                    const float d) {
  return (a * x * x * x) + (b * x * x) + (c * x) + d;
}
void Scene::cubic(std::vector<glm::vec2> &points, const glm::vec4 params) {
  static constexpr auto disc = [](auto a, auto b, auto c) -> float {
    return b * b - 4 * a * c;
  };
  static constexpr auto roots = [](auto a, auto b, auto disc) -> float {
    return (-b + (a > 0 ? +1 : -1) * std::sqrt(disc)) / (2 * a);
  };
  const auto a = params.x, b = params.y, c = params.z, d = params.w;
  const auto aSign = a > 0 ? +1 : -1;

  // dy/dx = 3ax^2 + 2bx + c
  // A: pivot point of cubic is vertex of derivative
  // B: dy/dx = -1
  // C: dy/dx = 0
  // D: dy/dx = 1

  // for a > 0
  // AB: y fast -y
  // BC: x fast -y
  // CD: x fast +y
  // D+: y fast +y

  // for a < 0
  // AD: y fast +y
  // DC: x fast +y
  // CB: x fast -y
  // B+: y fast -y

  const int h = -b / (2 * a), k = cubicF(h, a, b, c, d);
  const glm::ivec2 vertex{h, k};

  if (a == 0) {
    quadratic(points, {b, c, d});
  } else {
    auto curr = vertex;
    if (const auto ds = disc(3 * a, 2 * b, c + aSign);
        ds >= 0) { // intersection at -aSign exists
      const int stop = roots(3 * a, 2 * b, ds);
      // y fast -aSign * y
      // [X][ ] [1][2]
      // [1][2] [X][ ]
      // fmt::println("x={} stop-1={}", curr.x, stop);
      auto p = cubicXY(curr.x + 0.5, curr.y - aSign, a, b, c, d);
      for (; curr.x < stop; curr.y -= aSign) {
        points.emplace_back(Scene::asBig(curr + bonusOffset));
        points.emplace_back(Scene::asBig(2 * vertex - curr + bonusOffset));

        // p(i) = f(x + 0.5, y -+ 1)
        //  = a(x^3 + 1.5x^2 + 0.75x + 0.125)
        //  + b(x^2 + x + 0.25)
        //  + c(x + 0.5)
        //  + d
        //  - (y -+ 1)
        if (aSign * p < 0) {
          // p(i + 1) = f(x + 0.5, y -+ 2)
          //  = a(x^3 + 1.5x^2 + 0.75x + 0.125)
          //  + b(x^2 + x + 0.25)
          //  + c(x + 0.5)
          //  + d
          //  - (y -+ 2)
          // p(i + 1) - p(i) = +- 1

          p += aSign;
        } else {
          // p(i + 1) = f(x + 1.5, y -+ 2)
          //  = a(x^3 + 4.5x^2 + 6.75x + 3.375)
          //  + b(x^2 + 3x + 2.25)
          //  + c(x + 1.5)
          //  + d
          //  - (y -+ 2)
          // p(i + 1) - p(i)
          //  = a(3x^2 + 6x + 3.25)
          //  + b(2x + 2)
          //  + c(1)
          //  +- 1

          p += a * quadraticF(curr.x, 3, 6, 3.25) + 2 * b * (curr.x + 1) + c +
               aSign;
          curr.x++;
        }
      }
    }
    if (const auto ds = disc(3 * a, 2 * b, c);
        ds >= 0) { // intersection at 0 exists
      const int stop = roots(3 * a, 2 * b, ds);
      // x fast -aSign * y
      // [X][1] [ ][2]
      // [ ][2] [X][1]
      // fmt::println("x={} stop0={}", curr.x, stop);
      auto p = cubicXY(curr.x + 1, curr.y - aSign * 0.5, a, b, c, d);
      for (; curr.x < stop; curr.x++) {
        points.emplace_back(Scene::asBig(curr + bonusOffset));
        points.emplace_back(Scene::asBig(2 * vertex - curr + bonusOffset));

        // p(i) = f(x + 1, y -+ 0.5)
        //  = a(x^3 + 3x^2 + 3x + 1)
        //  + b(x^2 + 2x + 1)
        //  + c(x + 1)
        //  + d
        //  - (y -+ 0.5)
        const auto common =
            a * quadraticF(curr.x, 3, 9, 7) + b * (2 * curr.x + 3) + c;
        if (aSign * p < 0) {
          // p(i + 1) = f(x + 2, y -+ 1.5)
          //  = a(x^3 + 6x^2 + 12x + 8)
          //  + b(x^2 + 4x + 4)
          //  + c(x + 2)
          //  + d
          //  - (y -+ 1.5)
          // p(i + 1) - p(i)
          //  = a(3x^2 + 9x + 7)
          //  + b(2x + 3)
          //  + c(1)
          //  +- 1

          // p = cubicXY(curr.x + 2, curr.y - aSign * 1.5, a, b, c, d);
          p += common + aSign;
          curr.y -= aSign;
        } else {
          // p(i + 1) = f(x + 2, y -+ 0.5)
          //  = a(x^3 + 6x^2 + 12x + 8)
          //  + b(x^2 + 4x + 4)
          //  + c(x + 2)
          //  + d
          //  - (y -+ 0.5)
          // p(i + 1) - p(i)
          // p(i + 1) - p(i)
          //  = a(3x^2 + 9x + 7)
          //  + b(2x + 3)
          //  + c(1)

          // p = cubicXY(curr.x + 2, curr.y - aSign * 0.5, a, b, c, d);
          p += common;
        }
      }
    }
    if (const auto ds = disc(3 * a, 2 * b, c - aSign);
        ds >= 0) { // intersection at aSign exists
      const int stop = roots(3 * a, 2 * b, ds);
      // x fast aSign * y
      // [ ][2] [X][1]
      // [X][1] [ ][2]
      // fmt::println("x={} stop+1={}", curr.x, stop);
      auto p = cubicXY(curr.x + 1, curr.y + aSign * 0.5, a, b, c, d);
      for (; curr.x < stop; curr.x++) {
        points.emplace_back(Scene::asBig(curr + bonusOffset));
        points.emplace_back(Scene::asBig(2 * vertex - curr + bonusOffset));

        // p(i) = f(x + 1, y +- 0.5)
        //  = a(x^3 + 3x^2 + 3x + 1)
        //  + b(x^2 + 2x + 1)
        //  + c(x + 1)
        //  + d
        //  - (y +- 0.5)
        const auto common =
            a * quadraticF(curr.x, 3, 9, 7) + b * (2 * curr.x + 3) + c;
        if (aSign * p < 0) {
          // p(i + 1) = f(x + 2, y +- 0.5)
          //  = a(x^3 + 6x^2 + 12x + 8)
          //  + b(x^2 + 4x + 4)
          //  + c(x + 2)
          //  + d
          //  - (y +- 0.5)
          // p(i + 1) - p(i)
          //  = a(3x^2 + 9x + 7)
          //  + b(2x + 3)
          //  + c(1)

          // p = cubicXY(curr.x + 2, curr.y + aSign * 0.5, a, b, c, d);
          p += common;
        } else {
          // p(i + 1) = f(x + 2, y +- 1.5)
          //  = a(x^3 + 6x^2 + 12x + 8)
          //  + b(x^2 + 4x + 4)
          //  + c(x + 2)
          //  + d
          //  - (y +- 1.5)
          // p(i + 1) - p(i)
          //  = a(3x^2 + 9x + 7)
          //  + b(2x + 3)
          //  + c(1)
          //  -+ 1

          // p = cubicXY(curr.x + 2, curr.y + aSign * 1.5, a, b, c, d);
          p += common - aSign;
          curr.y += aSign;
        }
      }
    }
    // y fast aSign * y
    // [1][2] [X][ ]
    // [X][ ] [1][2]
    auto p = cubicXY(curr.x + 0.5, curr.y + aSign, a, b, c, d);
    for (; std::abs(curr.y) <= app().framebufferSize.y; curr.y += aSign) {
      points.emplace_back(Scene::asBig(curr + bonusOffset));
      points.emplace_back(Scene::asBig(2 * vertex - curr + bonusOffset));

      // p(i) = f(x + 0.5 y +- 1)
      //  = a(x^3 + 1.5x^2 + 0.75x + 0.125)
      //  + b(x^2 + x + 0.25)
      //  + c(x + 0.5)
      //  + d
      //  - (y +- 1)

      if (aSign * p < 0) {
        // p(i + 1) = f(x + 1.5, y +- 2)
        //  = a(x^3 + 4.5x^2 + 6.75x + 3.375)
        //  + b(x^2 + 3x + 2.25)
        //  + c(x + 1.5)
        //  + d
        //  - (y +- 2)
        // p(i + 1) - p(i)
        //  = a(3x^2 + 6x + 3.25)
        //  + b(2x + 2)
        //  + c(1)
        //  -+ 1

        // p = cubicXY(curr.x + 1.5, curr.y + aSign * 2, a, b, c, d);
        p += a * quadraticF(curr.x, 3, 6, 3.25) + 2 * b * (curr.x + 1) + c -
             aSign;
        curr.x++;
      } else {
        // p(i + 1) = f(x + 0.5, y -+ 2)
        //  = a(x^3 + 1.5x^2 + 0.75x + 0.125)
        //  + b(x^2 + x + 0.25)
        //  + c(x + 0.5)
        //  + d
        //  - (y -+ 2)
        // p(i + 1) - p(i) = -+ 1

        // p = cubicXY(curr.x + 0.5, curr.y + aSign * 2, a, b, c, d);
        p -= aSign;
      }
    }
  }
}

const std::vector<glm::vec2> &Scene::generateBonus() {
  bonusPoints.clear();
  switch (bonusState) {
  case BonusState::NONE: {
    break;
  }
  case BonusState::QUADRATIC: {
    quadratic(bonusPoints, quadraticParams);
    break;
  }
  case BonusState::CUBIC: {
    cubic(bonusPoints, cubicParams);
    break;
  }
  case BonusState::SUPER_QUADRIC: {
    // technically superellipse
    break;
  }
  }
  return bonusPoints;
}