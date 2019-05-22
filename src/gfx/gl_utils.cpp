#include "gl_utils.hpp"

namespace gfx
{
    bool Utils::InitializeGLFW()
    {
        if (!glfwInit()) return false;
        return true;
    }

    bool Utils::InitializeGL()
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;
        return true;
    }

    GLFWwindow* Utils::CreateGLFWWindow(int width, int height, const char* title, int MSAASamples)
    {
        if (!InitializeGLFW())
        {
            MessageBoxA(nullptr, "Error initializing GLFW!", "gl::Utils", MB_OK | MB_ICONERROR);
            return nullptr;
        }
        glfwSetErrorCallback((GLFWerrorfun)PrintGLFWError);

        glfwWindowHint(GLFW_SAMPLES, MSAASamples);
        GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);

        if (window == nullptr)
        {
            MessageBoxA(nullptr, "Error initializing GLFW Window!", "gl::Utils", MB_OK | MB_ICONERROR);
            glfwTerminate();
            return nullptr;
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);

        if (!InitializeGL())
        {
            MessageBoxA(nullptr, "Error initializing GLAD!", "gl::Utils", MB_OK | MB_ICONERROR);
            return nullptr;
        }

        return window;
    }

    void Utils::Cleanup()
    {
        glfwTerminate();
    }

    void Utils::PrintGLFWError(int errorCode, const char* error)
    {
        MessageBoxA(nullptr, error, "GLFW Error", MB_OK | MB_ICONERROR);
    }
}