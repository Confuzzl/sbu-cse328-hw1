#include "scene.h"

#include <cmath>

static void bresenhamOctant(std::vector<glm::vec2> &points,
                            const glm::ivec2 start, const glm::ivec2 end,
                            const bool xFast) {
  // auto d = end - start;
  // auto p = 2 * d.y - d.x;
  // int y = iStart.y;
  // for (int x = iStart.x; x < iEnd.x; x += 1) {
  //   SEND POINTS
  //
  //   if (p < 0) {
  //     p += 2 * d.y;
  //   } else {
  //     y += 1;
  //     p += 2 * (d.y - d.x);
  //   }
  // }

  auto d = end - start;
  auto stepDirection = 1;
  if (d[xFast] < 0) {
    stepDirection = -1;
    d[xFast] *= -1;
  }

  auto p = (2 * d[xFast]) - d[!xFast];
  for (auto curr = start; curr[!xFast] < end[!xFast]; curr[!xFast]++) {
    points.emplace_back(Scene::asBig(curr));

    if (p < 0) {
      p += 2 * d[xFast];
    } else {
      curr[xFast] += stepDirection;
      p += 2 * (d[xFast] - d[!xFast]);
    }
  }
}
void Scene::bresenham(std::vector<glm::vec2> &points, const glm::ivec2 start,
                      const glm::ivec2 end) {
  const auto d = glm::abs(end - start);
  if (d.y < d.x) {
    if (start.x > end.x)
      bresenhamOctant(points, end, start, true);
    else
      bresenhamOctant(points, start, end, true);
  } else {
    if (start.y > end.y)
      bresenhamOctant(points, end, start, false);
    else
      bresenhamOctant(points, start, end, false);
  }
}

void Scene::midPointEllipse(std::vector<glm::vec2> &points,
                            const glm::ivec2 start, const glm::ivec2 semiSize) {
  // work from the top and towards the right
  const auto a = semiSize.x, b = semiSize.y;
  if (a == 0 || b == 0)
    return;
  const auto a2 = a * a, b2 = b * b;

  // 1 = x^2/a^2 + y^2/b^2
  // a^2b^2 = b^2x^2 + a^2y^2
  // f(x,y) = b^2x^2 + a^2y^2 - a^2b^2
  // dy/dx = -b^2x/a^2y
  static constexpr auto f = [](float a2, float b2, float x, float y) {
    return (b2 * x * x) + (a2 * y * y) - (a2 * b2);
  };

  // [X][1]
  // [2][3]
  glm::ivec2 curr{0, b};
  { // -1 <= slope < 0
    // -1 <= -b^2x/a^2y < 0
    // b^2x/a^2y <= 1
    // b^2x <= a^2y

    // check 1 and 3
    // p(i) = f(x + 1, y - 0.5)
    //  = b^2(x + 1)^2 + a^2(y - 0.5)^2 - a^2b^2
    //  = b^2x^2 + 2b^2x + b^2 + a^2y^2 - a^2y + 0.25a^2 - a^2b^2
    //
    // p(1) = f(1, b - 0.5) = b^2 + a^2(b-0.5)^2 - a^2b^2
    //  = b^2 + a^2(b^2-b+0.25) - a^2b^2
    //  = b^2 + a^2b^2 - a^2b + 0.25a^2 - a^2b^2
    //  = b^2 - a^2b + 0.25a^2
    auto p = b2 - a2 * curr.y + a2 / 4;
    while (b2 * curr.x <= a2 * curr.y) {
      for (const auto xm : {-1, 1}) {
        for (const auto ym : {-1, 1}) {
          points.emplace_back(Scene::asBig(start + curr * glm::ivec2{xm, ym}));
        }
      }
      if (p < 0) {
        // case 1:
        // p(i + 1) = f(x + 2, y - 0.5)
        //  = b^2(x + 2)^2 + a^2(y - 0.5)^2 - a^2b^2
        //  = b^2x^2 + 4b^2x + 4b^2 + a^2y^2 - a^2y + 0.25a^2 - a^2b^2
        // p(i + 1) - p(i)
        //  = 2b^2x + 3b^2
        //  = b^2(2x + 3)

        p += b2 * (2 * curr.x + 3);
      } else {
        // case 3:
        // p(i + 1) = f(x + 2, y - 1.5)
        //  = b^2(x + 2)^2 + a^2(y - 1.5)^2 - a^2b^2
        //  = b^2x^2 + 4b^2x + 4b^2 + a^2y^2 - 3a^2y + 2.25a^2 - a^2b^2
        // p(i + 1) - p(i)
        //  = 2b^2x + 3b^2 - 2a^2y + 2a^2
        //  = b^2(2x + 3) + 2a^2(1 - y)

        p += b2 * (3 + 2 * curr.x) + 2 * a2 * (1 - curr.y);

        curr.y--;
      }
      curr.x++;
    }
  }
  { // -inf < slope < -1

    // check 2 and 3
    // p(i) = f(x + 0.5, y - 1)
    //  = b^2(x + 0.5)^2 + a^2(y - 1)^2 - a^2b^2
    //  = b^2x^2 + b^2x + 0.25b^2 + a^2y^2 - 2a^2y + a^2 - a^2b^2
    auto p = f(a2, b2, curr.x + 0.5f, curr.y - 1.0f);
    while (curr.y >= 0) {
      for (const auto xm : {-1, 1}) {
        for (const auto ym : {-1, 1}) {
          points.emplace_back(Scene::asBig(start + curr * glm::ivec2{xm, ym}));
        }
      }
      if (p < 0) {
        // case 3:
        // p(i + 1) = f(x + 1.5, y - 2)
        //  = b^2(x + 1.5)^2 + a^2(y - 2)^2 - a^2b^2
        //  = (b^2x^2 + 3b^2x + 2.25b^2) + (a^2y^2 - 4a^2y + 4a^2) - a^2b^2
        // p(i + 1) - p(i)
        //  = 2b^2x + 2b^2 - 2a^2y + 3a^2
        //  = 2b^2(x + 1) - a^2(2y + 3)

        p += 2 * b2 * (curr.x + 1) + a2 * (3 - 2 * curr.y);
        curr.x++;
      } else {
        // case 2:
        // p(i + 1) = f(x + 0.5, y - 2)
        //  = b^2(x + 0.5)^2 + a^2(y - 2)^2 - a^2b^2
        //  = (b^2x^2 + b^2x + 0.25b^2) + (a^2y^2 - 4a^2y + 4a^2) - a^2b^2
        // p(i + 1) - p(i)
        //  = -2a^2y + 3a^2
        //  = a^2(3 - 2y)

        p += a2 * (3 - 2 * curr.y);
      }
      curr.y--;
    }
  }
}
void Scene::midPointCircle(std::vector<glm::vec2> &points,
                           const glm::ivec2 start, const glm::ivec2 end) {
  const auto d = end - start;
  const auto r =
      static_cast<int>(std::sqrt(static_cast<float>(d.x * d.x + d.y * d.y)));

  // start at (r, 0)
  // f(x,y) = x^2 + y^2 - r^2
  // [2][1]
  // [ ][x]
  //
  // p(i) = f(x - 0.5, y + 1) = x^2 - x + 0.25 + y^2 + 2y + 1 - r^2
  // p(i) = x^2 - x + y^2 + 2y - r^2 + 1.25
  //
  // p(1) = f(r - 0.5, 1) = (r - 0.5)^2 + 1 - r^2 = r^2 - r + 0.25 + 1 - r^2
  // p(1) = 1.25 - r
  auto p = 1 - r;
  for (glm::ivec2 curr{r, 0}; curr.x >= curr.y; curr.y++) {
    const glm::ivec2 swapped{curr.y, curr.x};
    for (const auto pair : {curr, swapped}) {
      for (const auto xm : {-1, 1}) {
        for (const auto ym : {-1, 1}) {
          points.emplace_back(Scene::asBig(start + pair * glm::ivec2{xm, ym}));
        }
      }
    }

    if (p < 0) {
      // case 1:
      // p(i + 1) = f(x - 0.5, y + 2)
      //  = (x - 0.5)^2 + (y + 2)^2 - r^2
      //  = x^2 - x + 0.25 + y^2 + 4y + 4 - r^2
      //  = x^2 - x + y^2 + 4y - r^2 + 4.25
      // p(i + 1) - p(i)
      //  = 2y + 3
      p += 2 * curr.y + 3;
    } else {
      // case 2:
      // p(i + 1) = f(x + 0.5, y + 2)
      //  = (x + 0.5)^2 + (y + 2)^2 - r^2
      //  = x^2 - 3x + 2.25 + y^2 + 4y + 4 -r^2
      //  = x^2 - 3x + y^2 + 4y - r^2 + 6.25
      // p(i + 1) - p(i)
      //  = -2x + 2y + 5
      //  = 2(y - x) + 5
      p += 2 * (curr.y - curr.x) + 5;
      curr.x--;
    }
  }
}
const std::vector<glm::vec2> &Scene::generatePreview() {
  previewPoints.clear();
  switch (toolstate) {
  case ToolState::LINE: {
    bresenham(previewPoints, smallStart, smallPreview);
    break;
  }
  case ToolState::POLYLINE: {
    for (auto i = 0; i < polyPoints.size() - 1; i++) {
      bresenham(previewPoints, polyPoints[i], polyPoints[i + 1]);
    }
    bresenham(previewPoints, polyPoints.back(), smallPreview);
    if (connectPoly) {
      bresenham(previewPoints, polyPoints.front(), smallPreview);
    }
    break;
  }
  case ToolState::ELLIPSE: {
    if (circle)
      midPointCircle(previewPoints, smallStart, smallPreview);
    else
      midPointEllipse(previewPoints, smallStart,
                      glm::abs(smallPreview - smallStart));
    break;
  }
  }
  return previewPoints;
}