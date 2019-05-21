#include <glad/glad.h>

#include "gfx/camera.hpp"
#include "gfx/iv_buffer.hpp"
#include "gfx/shaders.hpp"
#include "gfx/textures.hpp"
#include "minecraft/chunk.hpp"
#include "minecraft/simplex1234.hpp"
#include "models/cube.hpp"
#include "window/window.hpp"

#include <imgui.h>
#include "imgui/imgui_impl_glfw.hpp"
#include "imgui/imgui_impl_opengl3.hpp"
#include "imgui/imgui_fonts.hpp"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

gfx::camera* cam;
gfx::texture2D* atlas;
fonts::font_handler* fontHandler;
window::Window *mainWindow;

std::vector<minecraft::Chunk> chunks;

using minecraft::BlockType;

void CleanupPointers()
{
    delete cam, atlas, fontHandler, mainWindow;
}

const int X_CHUNKS = 12;
const int Z_CHUNKS = 12;
void loadAssets(GLFWwindow* window)
{
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

    // Chunk
    chunks = std::vector<minecraft::Chunk>{};
    for (int xi = 0; xi < X_CHUNKS; xi++)
    {
        for (int zi = 0; zi < Z_CHUNKS; zi++)
        {
            minecraft::Chunk temp = minecraft::Chunk(glm::vec3(xi * 16, 0, zi * 16));
            for (int x = 0; x < 16; x++)
            {
                int X = x + 16 * xi;
                for (int z = 0; z < 16; z++)
                {
                    int Z = z + 16 * zi;
                    float n = SimplexNoise1234::noise(
                        static_cast<float>(X) * 0.01,
                        static_cast<float>(Z) * 0.01);
                    float n2 = SimplexNoise1234::noise(
                        static_cast<float>(X) * 0.005 + 567.0,
                        static_cast<float>(Z) * 0.005 + 880.0);
                    int h = 30 + static_cast<int>(20 * n2 * n);
                    for (int y = h; y >= 0; y--)
                    {
                        BlockType type = BlockType::Stone;
                        if (y == h && SimplexNoise1234::noise(static_cast<float>(X) * 0.125 + 50.0, static_cast<float>(Z) * 0.125 + 50.0) >= -0.25) type = BlockType::Grass;
                        else if (y >= h - 4) type = BlockType::Dirt;
                        temp.SetBlockAt(type, x, y, z);
                    }
                }
            }
            temp.CreateInstanceData();
            chunks.push_back(temp);
        }
    }
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

void CalculateMoveAxis(float moveSpeed)
{
	if (wHeld) Move(glm::vec2(0.0f, moveSpeed));
	if (sHeld) Move(glm::vec2(0.0f, -moveSpeed));
	if (aHeld) Move(glm::vec2(moveSpeed, 0.0f));
	if (dHeld) Move(glm::vec2(-moveSpeed, 0.0f));
}

// --

unsigned int cubeBuf;
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

gfx::color fogColor = gfx::color("#7DADC4");
float fogDensity = 0.007;
gfx::color lightColor = gfx::color("#E8FFFE");
gfx::color ambientColor = gfx::color("#5A5A5A");
glm::vec3 lightDir = glm::vec3(64.0f, -110.0f, 64.0f);
float specularStrength = 1.0f;
int main()
{
    mainWindow = new window::Window(800, 600, "Minecraft C++");
    
    loadAssets(mainWindow->GetHandle());

    glfwSetKeyCallback(mainWindow->GetHandle(), KeyCallback);

	// Init mouse
	glfwSetInputMode(mainWindow->GetHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    mainWindow->AddRenderAction([&](GLFWwindow* window) {
        if (hide) CalculateLookAxis();
        CalculateMoveAxis(0.25f);
    });

    mainWindow->AddRenderAction([&](GLFWwindow* window) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDisable(GL_MULTISAMPLE);
        glEnable(GL_CULL_FACE);

        for (int i = 0; i < chunks.size(); i++)
        {
            cam->recalculate(&(chunks[i].GetShader()));
            chunks[i].GetShader().setColor("LightColor", lightColor);
            chunks[i].GetShader().setColor("AmbientColor", ambientColor);
            chunks[i].GetShader().setVector("LightDir", lightDir);
            chunks[i].GetShader().setFloat("SpecularStrength", specularStrength);
            chunks[i].GetShader().setColor("FogColor", fogColor);
            chunks[i].GetShader().setFloat("FogDensity", fogDensity);
            chunks[i].Draw(*atlas);
        }
    });

    double previousFrameTime = glfwGetTime();
    mainWindow->AddRenderAction([&](GLFWwindow* window) {
        UI_BeginFrame();

        fontHandler->use("Roboto", 14.f);

        ImGui::Begin("Render Data");
        long instanceCount = 0;
        for (int i = 0; i < chunks.size(); i++)
        {
            instanceCount += chunks[i].GetInstanceCount();
        }
        ImGui::TextColored(ImVec4(UNPACK_COLOR3(gfx::color("#42aaf4")), 1.0), ("Chunks: " + std::to_string(chunks.size())).c_str());
        ImGui::TextColored(ImVec4(UNPACK_COLOR3(gfx::color("#f2ad0e")), 1.0), ("Instances: " + std::to_string(instanceCount)).c_str());
        ImGui::TextColored(ImVec4(UNPACK_COLOR3(gfx::color("#0ef174")), 1.0), ("FPS: " + std::to_string(1.0 / (glfwGetTime() - previousFrameTime))).c_str());
        ImGui::End();

        ImGui::Begin("Lighting");
        ImGui::ColorEdit3("Light Color", (float*)&lightColor);
        ImGui::ColorEdit3("Ambient Color", (float*)&ambientColor);
        ImGui::SliderFloat3("Light Direction", glm::value_ptr(lightDir), 0.0, 128);
        ImGui::SliderFloat("Specular Strength", &specularStrength, 0.0, 1.0);
        ImGui::Separator();
        ImGui::ColorEdit3("Fog Color", (float*)&fogColor);
        ImGui::SliderFloat("Fog Density", &fogDensity, 0.0, 0.125);
        ImGui::End();

        fontHandler->pop_all();

        UI_EndFrame();
        previousFrameTime = glfwGetTime();
    });

    mainWindow->SetClearColor(gfx::color("#96e6ff"));
    mainWindow->Run();

    CleanupPointers();

    return 0;
}