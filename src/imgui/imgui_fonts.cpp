#include "imgui_fonts.hpp"

fonts::font_handler::font_handler(std::initializer_list<font_mapping> mappings)
{
    std::vector<font_mapping> mapList{ mappings };
    for (unsigned int i = 0; i < mapList.size(); i++)
    {
        if (fonts.find(mapList[i].fontName) == fonts.end())
        {
            fonts[mapList[i].fontName] = ui_font{};
        }
        for (unsigned int j = 0; j < mapList[i].requestedSizes.size(); j++)
        {
            ImGuiIO& io = ImGui::GetIO();
            fonts[mapList[i].fontName].ptrs[mapList[i].requestedSizes[j]] = io.Fonts->AddFontFromFileTTF(mapList[i].filePath.c_str(), mapList[i].requestedSizes[j]);
        }
    }
}

void fonts::font_handler::use(std::string fontName, float fontSize)
{
    ImGui::PushFont(fonts[fontName].ptrs[fontSize]);
    pops++;
}

void fonts::font_handler::pop()
{
    ImGui::PopFont();
    pops--;
}

void fonts::font_handler::pop_all()
{
    for (; pops > 0; pops--) ImGui::PopFont();
}