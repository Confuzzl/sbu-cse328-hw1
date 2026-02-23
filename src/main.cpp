#include "app.h"

#include <GLFW/glfw3.h>

#include <cstdlib>
#include <stdexcept>

#include "util.h"

int main() {
  try {
    app().run();
  } catch (const std::runtime_error &e) {
    print_err(e.what());
  }
}
