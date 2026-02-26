#version 450 core

uniform uint frag_color;

out vec4 color;

vec4 rgba() {
	const uint r = frag_color >> 24u & 0xffu;
	const uint g = frag_color >> 16u & 0xffu;
	const uint b = frag_color >> 8u & 0xffu;
	const uint a = frag_color & 0xffu;
	return vec4(r, g, b, a) / 255.0;
}

void main() {
	color = rgba();
}
