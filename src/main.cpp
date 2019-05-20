#include <glad/glad.h>

#include "gfx/iv_buffer.hpp"
#include "gfx/shaders.hpp"
#include "gfx/textures.hpp"
#include "models/cube.hpp"
#include "window/window.hpp"

#include <imgui.h>
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_fonts.hpp"

gfx::iv_buffer<>* cube;
gfx::shader* cubeShader;
gfx::texture2D* atlas;
fonts::font_handler* fontHandler;

void loadAssets(GLFWwindow* window)
{
    // Buffers
    cube = new gfx::iv_buffer<>(cube_data::indices, ARRAYSIZE(cube_data::indices), cube_data::vertices, ARRAYSIZE(cube_data::vertices), []() {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    });

    // Shaders
    cubeShader = new gfx::shader("shaders/cube.vs", "shaders/cube.fs");
    cubeShader->compile();
    cubeShader->setTexture("Atlas", 0);

    // Textures
    atlas = new gfx::texture2D("res/atlas.png");
    atlas->load();

    // UI
    ImGui::CreateContext();
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui_ImplGlfw_InitForOpenGL(window, false);

    // Fonts
    fontHandler = new fonts::font_handler({
        {"Roboto", "res/Roboto-Regular.ttf", {14.f}}
    });
}

void UI_BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UI_EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

const char* blockNames[]{
    "Dirt", "Grass", "Stone", "No Block"
};
int AtlasIDX = 0;
int BreakIDX = 0;
int main()
{
    window::Window mainWindow = window::Window(800, 600, "Minecraft C++");
    
    loadAssets(mainWindow.GetHandle());

    mainWindow.AddRenderAction([&](GLFWwindow* window) {
        cubeShader->use();
        cubeShader->setInt("AtlasIDX", AtlasIDX);
        cubeShader->setInt("BreakIDX", BreakIDX);
        atlas->slot(GL_TEXTURE0);
        cube->Draw(GL_UNSIGNED_INT);
    });

    mainWindow.AddRenderAction([&](GLFWwindow* window) {
        UI_BeginFrame();

        fontHandler->use("Roboto", 14.f);
        ImGui::Begin("Block Render Props");
        ImGui::Combo("Type", &AtlasIDX, blockNames, IM_ARRAYSIZE(blockNames));
        ImGui::SliderInt("Break", &BreakIDX, 0, 7);
        ImGui::End();
        fontHandler->pop_all();

        UI_EndFrame();
    });

    mainWindow.SetClearColor(gfx::color("#96e6ff"));
    mainWindow.Run();

    delete cube, cubeShader, atlas;

    return 0;
}