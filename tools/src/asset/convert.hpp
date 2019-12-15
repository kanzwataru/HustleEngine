#pragma once

typedef int (*conversion_handler)(const char *, uint16_t);
int texture_convert(const char *name, uint16_t id);
int palette_convert(const char *name, uint16_t id);
int spritesheet_convert(const char *name, uint16_t id);
int tileset_convert(const char *name, uint16_t id);
int tilemap_convert(const char *name, uint16_t id);
