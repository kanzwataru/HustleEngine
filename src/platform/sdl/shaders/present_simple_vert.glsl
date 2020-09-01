#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_uv;

uniform vec2 screen_size;
uniform vec2 texture_size;

out vec2 uv;

void main() {
    vec2 pos = vec2(in_pos.x * 2 - 1, (1 - in_pos.y) * 2 - 1);

    gl_Position = vec4(pos, in_pos.z, 1.0);
    uv = vec2(in_uv.x, in_uv.y);
}
