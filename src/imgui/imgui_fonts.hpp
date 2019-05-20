#pragma once
#include <imgui.h>
#include <map>
#include <functional>
#include <string>
#include <vector>

namespace fonts
{
    struct font_mapping
    {
        std::string fontName;
        std::string filePath;
        std::vector<float> requestedSizes;
    };

    struct ui_font
    {
        std::map<float, ImFont*> ptrs{};
    };

    struct font_handler
    {
        std::map<std::string, ui_font> fonts{};
        font_handler(std::initializer_list<font_mapping> mappings);

        int pops = 0;
        void use(std::string fontName, float fontSize);
        void pop();
        void pop_all();
    };
}