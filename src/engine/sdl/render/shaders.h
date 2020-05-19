/* instanced */
static const char *planar_vert_inst_src =
"#version 330 core\n"
"layout (location = 0) in vec3 in_pos;\n"
"layout (location = 1) in vec2 in_uv;\n"
"layout (location = 2) in vec4 in_rect;\n"
"layout (location = 3) in vec4 in_data;\n"

"out vec2 uv;"
"out vec4 data;"

"uniform vec2 screen_size;\n"
"void main() {\n"
"   uv = in_uv;\n"
"   data = in_data;\n"

"   mat4 model = mat4(\n"
"       in_rect.z, 0, 0, 0,\n"
"       0, in_rect.w, 0, 0,\n"
"       0, 0, 1, 0,\n"
"       in_rect.x, in_rect.y, 0, 1\n"
"   );\n"

"   vec4 pos = vec4(in_pos.xyz, 1.0f);\n"
"   vec4 pixel_space = model * pos;\n"
"   vec4 screen_space = vec4(pixel_space.x / screen_size.x, pixel_space.y / screen_size.y, pixel_space.z, pixel_space.w);\n"
"   vec4 ndc_space = vec4(screen_space.x * 2 - 1, (1 - screen_space.y) * 2 - 1, screen_space.z, screen_space.w);\n"

"   gl_Position = ndc_space;\n"
"}\n\0"
;

static const char *flat_frag_inst_src =
"#version 330 core\n"
"in vec4 data;\n"
"out vec4 frag_col;\n"

"void main() {\n"
"   frag_col = vec4(data.x, 0, 0, 1);\n"
"}\n\0"
;

/* non-instanced */
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

"uniform float color_id;\n"

"void main() {\n"
"   frag_col = vec4(color_id, 0, 0, 1);\n"
"}\n\0"
;

static const char *post_vert_src =
"#version 330 core\n"
"layout (location = 0) in vec3 in_pos;\n"
"layout (location = 1) in vec2 in_uv;\n"

"out vec2 uv;\n"

"void main() {\n"
"   gl_Position = vec4(in_pos.x * 2 - 1, (1 - in_pos.y) * 2 - 1, in_pos.z, 1.0);\n"
"   uv = vec2(in_uv.x, 1 - in_uv.y);\n"
"}\n\0";

static const char *post_palette_frag_src =
"#version 330 core\n"
"in vec2 uv;\n"
"out vec4 frag_col;\n"

"uniform sampler1D palette;\n"
"uniform sampler2D pixel_target;\n"

"void main() {\n"
"   float id = texture(pixel_target, uv).r;\n"
"   frag_col = texture(palette, id);\n"
"}\n\0";

#if 0
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

"uniform sampler1D palette;\n"
"uniform float color_id;\n"

"void main()\n"
"{\n"
"   frag_col = texture(palette, color_id);\n"
"}\n\0";

static const char *sprite_frag_src =
"#version 330 core\n"
"in vec2 uv;\n"
"out vec4 frag_col;\n"

"uniform sampler1D palette;\n"
"uniform sampler2D sprite;\n"

"void main()\n"
"{\n"
"   float id = texture(sprite, uv).r;\n"
"   if(id == 0)\n"
"       discard;\n"
"   frag_col = texture(palette, id);\n"
"}\n\0";

static const char *post_vert_src =
"#version 330 core\n"
"layout (location = 0) in vec3 in_pos;\n"
"layout (location = 1) in vec2 in_uv;\n"

"out vec2 uv;\n"

"void main() {\n"
"   gl_Position = vec4(in_pos.x * 2 - 1, (1 - in_pos.y) * 2 - 1, in_pos.z, 1.0);\n"
"   uv = vec2(in_uv.x, 1 - in_uv.y);\n"
"}\n\0";

static const char *post_palette_frag_src =
"#version 330 core\n"
"in vec2 uv;\n"
"out vec4 frag_col;\n"

"uniform sampler2D pixel_target;\n"

"void main() {\n"
"   frag_col = texture(pixel_target, uv);\n"
"}\n\0";

static const char *text_frag_src = 
"#version 330 core\n"
"in vec2 uv;\n"
"out vec4 frag_col;\n"

"uniform vec4 uv_rect;\n"
"uniform vec3 color;\n"
"uniform sampler2D sprite;\n"

"void main()\n"
"{\n"
"   vec2 scaled_uv = (uv + uv_rect.xy) * uv_rect.zw;\n"
"   float id = texture(sprite, scaled_uv).r;\n"
"   if(id == 0)\n"
"       discard;\n"
"   else\n"
"       frag_col = vec4(color.rgb, 0);"
"}\n\0";

static const char *tilemap_frag_src = 
"#version 330 core\n"
"in vec2 uv;\n"
"out vec4 frag_col;\n"

"uniform vec4 uv_rect;\n"
"uniform sampler1D palette;\n"
"uniform sampler2D sprite;\n"

"void main()\n"
"{\n"
"   vec2 scaled_uv = (uv + uv_rect.xy) * uv_rect.zw;\n"
"   float id = texture(sprite, scaled_uv).r;\n"
"   if(id == 0)\n"
"       discard;\n"
"   frag_col = texture(palette, id);\n"
"}\n\0";

static const char *instanced_tile_vert_src =
"#version 330 core\n"
"layout (location = 0) in vec3 in_pos;\n"
"layout (location = 1) in vec2 in_uv;\n"
"layout (location = 2) in vec4 in_instance_data;\n"

"out vec2 uv;"

"uniform mat4 model;\n"
"uniform vec2 screen_size;\n"
"uniform vec2 uv_scale;\n"

"void main() {\n"
"   uv = (in_uv + in_uv_rect.xy) * in_uv_rect.zw;\n"

"   vec4 pos = vec4(in_pos.xyz, 1.0f);\n"
"   vec4 pixel_space = model * pos;\n"
"   vec4 screen_space = vec4(pixel_space.x / screen_size.x, pixel_space.y / screen_size.y, pixel_space.z, pixel_space.w);\n"
"   vec4 ndc_space = vec4(screen_space.x * 2 - 1, (1 - screen_space.y) * 2 - 1, screen_space.z, screen_space.w);\n"

"   gl_Position = ndc_space;\n"
"}\n\0";
#endif
