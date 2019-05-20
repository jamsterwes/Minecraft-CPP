#include <glad/glad.h>
#include "gfx/iv_buffer.hpp"
#include "gfx/shaders.hpp"
#include "gfx/textures.hpp"
#include "models/cube.hpp"
#include "window/window.hpp"

gfx::iv_buffer<>* cube;
gfx::shader* cubeShader;
gfx::texture2D* atlas;

void loadAssets()
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
}

int main()
{
    window::Window mainWindow = window::Window(800, 600, "Minecraft C++");
    
    loadAssets();

    mainWindow.AddRenderAction([&](GLFWwindow* window) {
        cubeShader->use();
        cubeShader->setInt("AtlasIDX", 0);
        atlas->slot(GL_TEXTURE0);
        cube->Draw(GL_UNSIGNED_INT);
    });

    mainWindow.SetClearColor(gfx::color("#96e6ff"));
    mainWindow.Run();

    delete cube, cubeShader, atlas;

    return 0;
}