cmake_minimum_required(VERSION 3.16)
project(hw1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)


add_executable(hw1
		"src/main.cpp"
		"src/app.cpp"
		"src/util.cpp" 
		"src/gl/uniform.cpp" 
		"src/gl/gl_object.cpp" 
		"src/gl/glsl_object.cpp"  
		"include/stb_image.h"
		"include/app.h"
		 
		"include/color.h" 
		"include/gl/gl_object.h" 
		"include/gl/glsl_object.h" 
		"include/gl/uniform.h" 
		"include/gl/vao.h" 
		"include/gl/vertex_layout.h" 
		"include/gl/component/vertex.h" 
		"include/gl/component/fragment.h" "include/gl/texture.h" "src/gl/texture.cpp" "include/renderer.h" "src/color.cpp" "src/gl/vertex_layout.cpp" "src/renderer.cpp" "include/callback.h" "src/callback.cpp" "include/input/key.h" "include/input/input_handler.h" "src/input/key.cpp" "src/input/input_handler.cpp" "include/scene.h" "src/scene.cpp" "src/scene_basic_impl.cpp" "src/scene_bonus_impl.cpp")
target_include_directories(hw1 PUBLIC include)
target_compile_definitions(hw1 PRIVATE SOURCE_DIR="${CMAKE_SOURCE_DIR}")

if (WIN32)
	list(APPEND CMAKE_PREFIX_PATH "C:/Program Files (x86)/GLFW/lib/cmake")
endif()
find_package(glfw3 REQUIRED)
target_link_libraries(hw1 glfw)

add_subdirectory("glad")
target_link_libraries(hw1 glad)

add_subdirectory("glm")
target_link_libraries(hw1 glm::glm)

add_subdirectory("imgui")
target_link_libraries(hw1 imgui)

add_subdirectory("fmt")
target_link_libraries(hw1 fmt::fmt)

if (CMAKE_DL_LIBS)
	target_link_libraries(hw1 ${CMAKE_DL_LIBS})
endif()