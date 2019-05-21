#include <glad/glad.h>

#include "gfx/camera.hpp"
#include "gfx/iv_buffer.hpp"
#include "gfx/shaders.hpp"
#include "gfx/textures.hpp"
#include "models/cube.hpp"
#include "window/window.hpp"

#include <imgui.h>
#include "imgui/imgui_impl_glfw.hpp"
#include "imgui/imgui_impl_opengl3.hpp"
#include "imgui/imgui_fonts.hpp"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

gfx::camera* cam;
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

    // Camera
    cam = new gfx::camera(60);

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

window::Window *mainWindow;

const char* blockNames[]{
    "Dirt", "Grass", "Stone", "No Block"
};
int AtlasIDX = 0;
int BreakIDX = 0;
glm::vec3 rotation = glm::vec3(0.0f);
int main()
{
    mainWindow = new window::Window(800, 600, "Minecraft C++");
    
    loadAssets(mainWindow->GetHandle());

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

    mainWindow->AddRenderAction([&](GLFWwindow* window) {
        cubeShader->use();
        cubeShader->setVector("AtlasIDX", glm::vec2(AtlasIDX, 0));
        cubeShader->setInt("BreakIDX", BreakIDX);
        atlas->slot(GL_TEXTURE0);
        cam->recalculate(cubeShader);

        // Model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z) * model;
        cubeShader->setMatrix("model", model);

        cube->Draw(GL_UNSIGNED_INT);
    });

    mainWindow->AddRenderAction([&](GLFWwindow* window) {
        UI_BeginFrame();

        fontHandler->use("Roboto", 14.f);
        ImGui::Begin("Block Render Props");
        ImGui::Combo("Type", &AtlasIDX, blockNames, IM_ARRAYSIZE(blockNames));
        ImGui::SliderInt("Break", &BreakIDX, 0, 7);
        ImGui::End();

        ImGui::Begin("Camera Pos");
        ImGui::SliderFloat3("Position", glm::value_ptr(cam->camPos), -10.0, 10.0);
        ImGui::SliderFloat3("Rotation", glm::value_ptr(rotation), -6.28, 0.0);
        ImGui::End();
        fontHandler->pop_all();

        UI_EndFrame();
    });

    mainWindow->SetClearColor(gfx::color("#96e6ff"));
    mainWindow->Run();

    delete cube, cubeShader, atlas, cam, mainWindow, fontHandler;

    return 0;
}