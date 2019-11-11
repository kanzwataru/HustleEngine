begin(Texture)
{
    .name = "roy",
    .path = "data/test.bmp",
    .width = 128,
    .height = 128
},
end(Texture)

/*
begin(Palette)
{
    .name = "pal",
    .path = "data/test.bmp",
},
end(Palette)
*/

none(Palette)

begin(Package)
{
    .name = "main",
    .contents = (const char *[]){
        "roy",
        //"pal",
        0,
    }
}
end(Package)
