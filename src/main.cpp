#include <glad/glad.h>
#include "window/window.hpp"

int main()
{
    window::Window mainWindow = window::Window(800, 600, "Minecraft C++");
    mainWindow.SetClearColor(colors::color("#96e6ff"));
    mainWindow.Run();
    return 0;
}