#pragma once
#include <GLFW/glfw3.h>
#include <functional>
#include <string>
#include <vector>
#include "../gfx/colors.hpp"

namespace window
{
    typedef std::function<void(GLFWwindow* window, int key, int action, int mods)> keyFunction;
    typedef std::function<void(GLFWwindow* window)> renderFunction;
    typedef std::function<void(int width, int height)> resizeFunction;

    class Window
    {
    public:
        Window();
        Window(int width, int height, std::string title);
        ~Window();

        void AddRenderAction(renderFunction action);
        void AddResizeAction(resizeFunction action);
        GLFWwindow* GetHandle();
        void Run();
        void SetClearColor(gfx::color newClearColor);
        void SetTitle(std::string newTitle);
        void SetSize(int width, int height);
        // Make getter later
        int width, height;
    private:
        void CommonConstructor(int width, int height, std::string title);

        GLFWwindow* glfwRef;
        std::vector<renderFunction> loopActions;
        std::vector<resizeFunction> resizeActions;
    };
}