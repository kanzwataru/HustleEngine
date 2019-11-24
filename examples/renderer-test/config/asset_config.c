begin(Texture)
{
    .name = "roy",
    .path = "data/test.bmp",
    .width = 128,
    .height = 128
},
end(Texture)

begin(Palette)
{
    .name = "pal",
    .path = "data/test.bmp",
},
end(Palette)

begin(Spritesheet)
{
    .name      = "char_run",
    .path      = "data/char_run.bmp",
    .width     = 8,
    .height    = 16,
    .count     = 6,
    .frameskip = 4
},
{
    .name      = "large_run",
    .path      = "data/large_run.bmp",
    .width     = 16,
    .height    = 32,
    .count     = 6,
    .frameskip = 4
},
end(Spritesheet)

begin(Package)
{
    .name = "main",
    .contents = (const char *[]){
        "roy",
        "pal",
        "char_run",
        "large_run",
        0,
    }
}
end(Package)
