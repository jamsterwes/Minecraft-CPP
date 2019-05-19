#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

namespace gl
{
    class Utils
    {
    public:
        static bool InitializeGLFW();
        static bool InitializeGL();
        static GLFWwindow* CreateGLFWWindow(int width, int height, const char* title, int MSAASamples);
        static std::string GetVersionString();
        static void Cleanup();
    private:
        static void PrintGLFWError(int errorCode, const char* error);
    };
}