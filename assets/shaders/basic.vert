#version 450 core

layout (location = 0) in vec2 pos;

layout (std140) uniform ViewBlock {
	mat4 view;
};

void main() {
	gl_Position = view * vec4(pos, 0.0, 1.0);
}
