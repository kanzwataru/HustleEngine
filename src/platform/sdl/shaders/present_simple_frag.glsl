#version 330 core
in vec2 uv;
out vec4 frag_col;

uniform vec2 texture_size;
uniform sampler2D pixel_target;

void main() {
    frag_col = texture(pixel_target, uv);
}
