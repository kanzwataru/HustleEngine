static const char *planar_vert_src =
"#version 330 core\n"
"layout (location = 0) in vec3 in_pos;\n"
"layout (location = 1) in vec2 in_uv;\n"

"out vec2 uv;"

"uniform mat4 model;\n"
"uniform vec2 screen_size;\n"

"void main() {\n"
"   uv = in_uv;\n"

"   vec4 pos = vec4(in_pos.xyz, 1.0f);\n"
"   vec4 pixel_space = model * pos;\n"
"   vec4 screen_space = vec4(pixel_space.x / screen_size.x, pixel_space.y / screen_size.y, pixel_space.z, pixel_space.w);\n"
"   vec4 ndc_space = vec4(screen_space.x * 2 - 1, (1 - screen_space.y) * 2 - 1, screen_space.z, screen_space.w);\n"

"   gl_Position = ndc_space;\n"
"}\n\0";

static const char *flat_frag_src =
"#version 330 core\n"
"out vec4 frag_col;\n"

"uniform vec3 color;\n"

"void main()\n"
"{\n"
"   frag_col = vec4(color, 1.0);\n"
"}\n\0";

static const char *post_vert_src =
"#version 330 core\n"
"layout (location = 0) in vec3 in_pos;\n"
"layout (location = 1) in vec2 in_uv;\n"

"out vec2 uv;\n"

"void main() {\n"
"   gl_Position = vec4(in_pos.x * 2 - 1, (1 - in_pos.y) * 2 - 1, in_pos.z, 1.0);\n"
//"   gl_Position = vec4(in_pos.x, in_pos.y, 0.0, 1.0);\n"
"   uv = in_uv;\n"
"}\n\0";

static const char *post_palette_frag_src =
"#version 330 core\n"
"in vec2 uv;\n"
"out vec4 frag_col;\n"

"uniform sampler2D indexed_pass;\n"

"void main() {\n"
//"   frag_col = texture(indexed_pass, uv);\n"
"   frag_col = vec4(uv.x, uv.y, 0, 1);\n"
"}\n\0";
