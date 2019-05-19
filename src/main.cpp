#include <glad/glad.h>
#include "window/window.hpp"

int main()
{
    window::Window mainWindow = window::Window(800, 600, "Minecraft C++");
    glClearColor(0.0f, 0.25f, 1.0f, 1.0f);
    mainWindow.Run();
    return 0;
}