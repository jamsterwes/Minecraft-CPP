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

// --

bool wHeld = false;
bool sHeld = false;
bool aHeld = false;
bool dHeld = false;

void CalculateLookAxis()
{
	double xpos, ypos;
	glfwGetCursorPos(mainWindow->GetHandle(), &xpos, &ypos);
	float dx = xpos - ((float)mainWindow->width / 2.0f);
	dx /= (float)mainWindow->width;
	float dy = -ypos + ((float)mainWindow->height / 2.0f);
	dy /= (float)mainWindow->height;
	cam->lookAxis.x = dx * 360.0f;
	cam->lookAxis.y = dy * 360.0f;
	
	if (cam->lookAxis.y > 89.0f) cam->lookAxis.y = 89.0f;
	if (cam->lookAxis.y < -89.0f) cam->lookAxis.y = -89.0f;
}

void Move(glm::vec2 move)
{
	glm::mat4 viewMat = glm::inverse(cam->view);
	glm::vec3 forward = glm::normalize(glm::vec3(viewMat[2][0], viewMat[2][1], viewMat[2][2]));
	glm::vec3 right = glm::normalize(glm::vec3(viewMat[0][0], viewMat[0][1], viewMat[0][2]));
	cam->camPos += move.y * -forward + move.x * -right;
}

void CalculateMoveAxis()
{
	if (wHeld) Move(glm::vec2(0.0f, 1.0f));
	if (sHeld) Move(glm::vec2(0.0f, -1.0f));
	if (aHeld) Move(glm::vec2(1.0f, 0.0f));
	if (dHeld) Move(glm::vec2(-1.0f, 0.0f));
}

// --

unsigned int cubeBuf;
int SetupCubes();

bool hide = true;
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
        hide = !hide;
        // Init mouse
        glfwSetInputMode(mainWindow->GetHandle(), GLFW_CURSOR, hide ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		wHeld = true;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		sHeld = true;
	}
	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
	{
		wHeld = false;
	}
	if (key == GLFW_KEY_S && action == GLFW_RELEASE)
	{
		sHeld = false;
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		aHeld = true;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		dHeld = true;
	}
	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
	{
		aHeld = false;
	}
	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
	{
		dHeld = false;
	}
}

int BreakIDX = 0;
glm::vec3 rotation = glm::vec3(0.0f);
int main()
{
    mainWindow = new window::Window(800, 600, "Minecraft C++");
    
    loadAssets(mainWindow->GetHandle());
    int instanceCount = SetupCubes();

    glfwSetKeyCallback(mainWindow->GetHandle(), KeyCallback);

	// Init mouse
	glfwSetInputMode(mainWindow->GetHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    mainWindow->AddRenderAction([&](GLFWwindow* window) {
        if (hide) CalculateLookAxis();
        CalculateMoveAxis();
    });

    mainWindow->AddRenderAction([&](GLFWwindow* window) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDisable(GL_MULTISAMPLE);
        glEnable(GL_CULL_FACE);

        cubeShader->use();
        cubeShader->setInt("BreakIDX", BreakIDX);
        atlas->slot(GL_TEXTURE0);
        cam->recalculate(cubeShader);

        // Model matrix
        glm::mat4 model = glm::mat4(1.0f);
        cubeShader->setMatrix("model", model);

        cube->Draw(GL_UNSIGNED_INT, instanceCount);
    });

    double previousFrameTime = glfwGetTime();
    mainWindow->AddRenderAction([&](GLFWwindow* window) {
        UI_BeginFrame();

        fontHandler->use("Roboto", 14.f);
        ImGui::Begin("Block Render Props");
        ImGui::TextColored(ImVec4(UNPACK_COLOR3(gfx::color("#f2ad0e")), 1.0), ("Instances: " + std::to_string(instanceCount)).c_str());
        ImGui::TextColored(ImVec4(UNPACK_COLOR3(gfx::color("#0ef174")), 1.0), ("FPS: " + std::to_string(1.0 / (glfwGetTime() - previousFrameTime))).c_str());
        ImGui::SliderInt("Break", &BreakIDX, 0, 7);
        ImGui::SliderFloat3("Rotation", glm::value_ptr(rotation), -6.28, 0.0);
        ImGui::End();

        fontHandler->pop_all();

        UI_EndFrame();
        previousFrameTime = glfwGetTime();
    });

    mainWindow->SetClearColor(gfx::color("#96e6ff"));
    mainWindow->Run();

    delete cube, cubeShader, atlas, cam, mainWindow, fontHandler;

    return 0;
}

struct cube_inst_data
{
    glm::vec3 pos;
    glm::vec2 uv;
};

int SetupCubes()
{
    std::vector<cube_inst_data> translations;
    for (int z = 0; z < 128; z++)
    {
        for (int x = 0; x < 128; x++)
        {
            int h = int(4 * sin(x * 0.125) * cos(z * 0.125)) + 32.0f;
            translations.push_back({
                glm::vec3(x, h, z), GRASS
            });
            for (int y = h - 1; y > h - 8; y--)
            {
                translations.push_back({
                    glm::vec3(x, y, z), DIRT
                });
            }
            for (int y = h - 8; y > 0; y--)
            {
                translations.push_back({
                    glm::vec3(x, y, z), STONE
                });
            }
        }
    }

    glBindVertexArray(cube->vaoID);
    glGenBuffers(1, &cubeBuf);
    glBindBuffer(GL_ARRAY_BUFFER, cubeBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_inst_data) * translations.size(), &translations[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glBindVertexArray(0);

    return translations.size();
}