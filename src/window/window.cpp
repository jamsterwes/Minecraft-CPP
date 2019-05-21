#include <glad/glad.h>
#include "window.hpp"
#include "../gfx/gl_utils.hpp"
#include <algorithm>

void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    window::Window* win = (window::Window*)glfwGetWindowUserPointer(window);
    win->SetSize(width, height);
}

namespace window
{
    Window::Window()
    {
        this->CommonConstructor(800, 600, "Minecraft C++");
    }

    Window::Window(int width, int height, std::string title)
    {
        this->CommonConstructor(width, height, title);
    }

    Window::~Window()
    {
        glfwDestroyWindow(glfwRef);
    }

    void Window::AddRenderAction(renderFunction action)
    {
        loopActions.push_back(action);
    }

    GLFWwindow* Window::GetHandle()
    {
        return this->glfwRef;
    }
    
    void Window::Run()
    {
        while (!glfwWindowShouldClose(glfwRef))
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            std::for_each(loopActions.begin(), loopActions.end(), [&](renderFunction fn) {
                fn(glfwRef);
            });

            glfwSwapBuffers(glfwRef);
            glfwPollEvents();
        }
    }
    
    void Window::SetClearColor(gfx::color newClearColor)
    {
        glClearColor(UNPACK_COLOR3(newClearColor), 1.0f);
    }

    void Window::SetTitle(std::string newTitle)
    {
        int major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        glfwSetWindowTitle(glfwRef, (newTitle + " [OpenGL " + std::to_string(major) + "." + std::to_string(minor) + "]").c_str());
    }

    void Window::SetSize(int width, int height)
    {
        this->width = width;
        this->height = height;
        glViewport(0, 0, width, height);
    }

    void Window::CommonConstructor(int width, int height, std::string title)
    {
        glfwRef = gfx::Utils::CreateGLFWWindow(width, height, "Temporary window title...", 8);
        glfwSetWindowUserPointer(glfwRef, (void*)(this));
        glfwSetFramebufferSizeCallback(glfwRef, frameBufferSizeCallback);
        this->SetSize(width, height);
        this->SetTitle(title);
        loopActions = std::vector<renderFunction>();
    }
}