#pragma once

#include "gl/uniform.h"

#define NEW_UNIFORM(type, name) GL::Uniform<type> name { programID, #name }
#define NEW_UNIFORM_BLOCK(type)  GL::UniformBlock<shaders::uniforms::type> type { programID }
