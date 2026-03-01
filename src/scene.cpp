#include "scene.h"

#include "app.h"

Scene::Scene()
    : bonusOffset{200, 100}, quadraticParams{-0.01, 0, 0},
      cubicParams{-0.005, 0.175, 0, 0}, superParams{1, 1, 1} {
  commitedPoints.reserve(INITIAL_POINT_CAPACITY);
  previewPoints.reserve(INITIAL_POINT_CAPACITY);
  bonusPoints.reserve(INITIAL_POINT_CAPACITY);

  polyPoints.reserve(256);
}
void Scene::addPolyPoint(const glm::ivec2 p) {
  if (!startSet()) {
    smallStart = p;
  }
  polyPoints.emplace_back(p);
}

void Scene::commitEndPoints() {
  commitedPoints.insert(commitedPoints.end(),
                        std::make_move_iterator(previewPoints.begin()),
                        std::make_move_iterator(previewPoints.end()));
  previewPoints.clear();
  polyPoints.clear();
  resetStart();
}

void Scene::setState(const ToolState next) {
  resetStart();
  toolstate = next;
}
