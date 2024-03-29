#include <glad/glad.h>
#include <cstdlib>
#include <ctime>

#include "gfx/camera.hpp"
#include "gfx/iv_buffer.hpp"
#include "gfx/shaders.hpp"
#include "gfx/textures.hpp"
#include "lighting/deferred.hpp"
#include "lighting/settings.hpp"
#include "minecraft/chunk.hpp"
#include "minecraft/mesh_renderer.hpp"
#include "minecraft/worldgen.hpp"
#include "models/cube.hpp"
#include "window/window.hpp"
#include "models/foliage.hpp"

#include <imgui.h>
#include "imgui/imgui_impl_glfw.hpp"
#include "imgui/imgui_impl_opengl3.hpp"
#include "imgui/imgui_fonts.hpp"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

gfx::camera* cam;
gfx::texture2D* atlas;
fonts::font_handler* fontHandler;
window::Window *mainWindow;

gfx::iv_buffer<>* foliage;

std::vector<minecraft::Chunk> chunks;

using minecraft::BlockType;

unsigned int GAME_SEED = 0;
void CleanupPointers()
{
    delete cam, atlas, fontHandler, mainWindow, foliage;
}

void SeedProbability(unsigned int seed = 0)
{
    if (seed == 0)
    {
        srand(time(0));
        GAME_SEED = rand();
    }
    srand(GAME_SEED);
}
bool CheckProbability(float probability)
{
    return ((float)(rand() % 100000) / 100000.0) < probability;
}

int ChunkDim[2] = {3, 1};
int chunkCount = ChunkDim[0] * ChunkDim[1];

int octaves = 7;
float frequency = 0.003174;
float lacunarity = 0.015;
float bias = -0.493f;
float upperScale = 0.974f;
float lowerScale = 0.00f;
int noiseBase = 80;
float treeProb = 0.049f;
bool wireframe = false;

std::list<minecraft::MeshRenderer*> meshRenderer;


const minecraft::BlockType grass = BlockType::TallGrass;
const minecraft::BlockType grassSurface = BlockType::Grass;
const minecraft::BlockType subsurface = BlockType::Dirt;
const minecraft::BlockType surface = BlockType::Grass;
const float grassProbability = 0.05f;
const bool makeFlowers = true;
const bool makeTrees = true;

// const minecraft::BlockType surface = BlockType::Sand;
// const minecraft::BlockType subsurface = BlockType::Sand;
// const minecraft::BlockType grass = BlockType::DeadBush;
// const minecraft::BlockType grassSurface = BlockType::Sand;
// const float grassProbability = 0.01f;
// const bool makeFlowers = false;
// const bool makeTrees = false;

void FillTreeLayer(minecraft::Chunk& chunk, int layer, int x, int y, int z);
void genChunks(bool worldGen = true)
{
    chunkCount = ChunkDim[0] * ChunkDim[1];
    meshRenderer = std::list<minecraft::MeshRenderer*>{};
    for (int i = 0; i < chunkCount; i++)
    {
        meshRenderer.push_back(new minecraft::MeshRenderer());
    }
    if (worldGen)
    {
        int seed = rand();
        auto meshRenderIt = meshRenderer.begin();
        for (int xi = 0; xi < ChunkDim[0]; xi++)
        {
            for (int zi = 0; zi < ChunkDim[1]; zi++)
            {
                minecraft::Chunk* temp = new minecraft::Chunk(glm::vec3(xi * CHUNK_SIZE, 0, zi * CHUNK_SIZE));
                for (int x = 0; x < CHUNK_SIZE; x++)
                {
                    int X = x + CHUNK_SIZE * xi;
                    for (int z = 0; z < CHUNK_SIZE; z++)
                    {
                        int Z = z + CHUNK_SIZE * zi;
                        int h = noiseBase + (int)(20.0 * minecraft::WorldGen::FBM(glm::vec2(X + seed, Z + seed), frequency, lacunarity, octaves, bias, upperScale, lowerScale));
                        for (int y = h; y >= 0; y--)
                        {
                            float antiTreeProb = 1.0f - treeProb;
                            float bedrockNoise = minecraft::WorldGen::FBM(glm::vec3(X, y, Z), frequency, lacunarity, octaves, 0.0, 1.0, 1.0);
                            float treeNoise = minecraft::WorldGen::FBM(glm::vec3(X, y, Z), 0.25, lacunarity / 20, 2, 0.0, 1.0, 1.0);
                            BlockType type = BlockType::Stone;
                            if (y <= 1 + bedrockNoise * 1)
                            {
                                type = BlockType::Bedrock;
                            } 
                            else if (y < h - 6) type = BlockType::Stone;
                            else if (y == h)
                            {
                                type = surface;
                                if (CheckProbability(treeProb / 20.0) && makeTrees)
                                {
                                    int max = h + 7 + treeNoise;
                                    for (int y2 = h + 1; y2 <= max; y2++)
                                    {
                                        FillTreeLayer(*temp, int(max - y2), x, y2, z);
                                    }
                                    type = BlockType::Dirt;
                                }
                                else if (CheckProbability(treeProb / 10.0) && makeFlowers)
                                {
                                    if (treeNoise > 0.5) temp->SetBlockAt(BlockType::Rose, x, y + 1, z);
                                    else temp->SetBlockAt(BlockType::Dandelion, x, y + 1, z);
                                    type = grassSurface;
                                }
                                else if (CheckProbability(grassProbability))
                                {
                                    temp->SetBlockAt(grass, x, y + 1, z);
                                    type = grassSurface;
                                }
                                else
                                {
                                    type = surface;
                                }
                            }
                            else if (y >= h - 6) type = subsurface;
                            temp->SetBlockAt(type, x, y, z);
                        }
                    }
                }
                temp->Consolidate();
                (*meshRenderIt)->AddChunk(*temp, 0);
                (*meshRenderIt)->Update();
                delete temp;
                meshRenderIt++;
            }
        }
    }
}

void genFakeOctreeChunks()
{
    chunkCount = ChunkDim[0] * ChunkDim[1];
    meshRenderer = std::list<minecraft::MeshRenderer*>{};
    auto meshRenderer0 = new minecraft::MeshRenderer{};
    minecraft::Chunk temp = minecraft::Chunk();
    meshRenderer0->AddChunk(temp);
    meshRenderer0->Update();
    meshRenderer.push_back(meshRenderer0);
}

void FillTreeLayer2n3(minecraft::Chunk& chunk, int x, int y, int z);
void FillTreeLayer(minecraft::Chunk& chunk, int layer, int x, int y, int z)
{
    switch(layer)
    {
    case 0:
        chunk.SetBlockAt(BlockType::Leaves, x, y, z);
        if (z + 1 < CHUNK_SIZE) chunk.SetBlockAt(BlockType::Leaves, x, y, z + 1);
        if (z - 1 >= 0) chunk.SetBlockAt(BlockType::Leaves, x, y, z - 1);
        if (x + 1 < CHUNK_SIZE) chunk.SetBlockAt(BlockType::Leaves, x + 1, y, z);
        if (x - 1 >= 0) chunk.SetBlockAt(BlockType::Leaves, x - 1, y, z);
        break;
    case 1:
        chunk.SetBlockAt(BlockType::Log, x, y, z);
        if (z + 1 < CHUNK_SIZE) chunk.SetBlockAt(BlockType::Leaves, x, y, z + 1);
        if (z - 1 >= 0) chunk.SetBlockAt(BlockType::Leaves, x, y, z - 1);
        if (x + 1 < CHUNK_SIZE) chunk.SetBlockAt(BlockType::Leaves, x + 1, y, z);
        if (x - 1 >= 0) chunk.SetBlockAt(BlockType::Leaves, x - 1, y, z);
        // 50%
        if (CheckProbability(0.5) && x + 1 < CHUNK_SIZE && z + 1 < CHUNK_SIZE) chunk.SetBlockAt(BlockType::Leaves, x + 1, y, z + 1);
        if (CheckProbability(0.5) && x - 1 > 0 && z - 1 > 0) chunk.SetBlockAt(BlockType::Leaves, x - 1, y, z - 1);
        if (CheckProbability(0.5) && x + 1 < CHUNK_SIZE && z - 1 > 0) chunk.SetBlockAt(BlockType::Leaves, x + 1, y, z - 1);
        if (CheckProbability(0.5) && x - 1 > 0 && z + 1 < CHUNK_SIZE) chunk.SetBlockAt(BlockType::Leaves, x - 1, y, z + 1);
        break;
    case 2:
        FillTreeLayer2n3(chunk, x, y, z);
        break;
    case 3:
        FillTreeLayer2n3(chunk, x, y, z);
        break;
    default:
        chunk.SetBlockAt(BlockType::Log, x, y, z);
    }
}

void FillTreeLayer2n3(minecraft::Chunk& chunk, int x, int y, int z)
{
    chunk.SetBlockAt(BlockType::Log, x, y, z);
    for (int xi = x - 2; xi <= x + 2; xi++)
    {
        for (int zi = z - 1; zi <= z + 1; zi++)
        {
            if (xi < CHUNK_SIZE && xi > 0 && zi > 0 && zi < CHUNK_SIZE) chunk.SetBlockAt(BlockType::Leaves, xi, y, zi);
        }
    }
    for (int xi = x - 1; xi <= x + 1; xi++)
    {
        for (int zi = z - 2; zi <= z + 2; zi++)
        {
            if (xi < CHUNK_SIZE && xi > 0 && zi > 0 && zi < CHUNK_SIZE) chunk.SetBlockAt(BlockType::Leaves, xi, y, zi);
        }
    }
    // 50%
    if (CheckProbability(0.5) && x + 2 < CHUNK_SIZE && z + 2 < CHUNK_SIZE) chunk.SetBlockAt(BlockType::Leaves, x + 2, y, z + 2);
    if (CheckProbability(0.5) && x - 2 > 0 && z - 2 > 0) chunk.SetBlockAt(BlockType::Leaves, x - 2, y, z - 2);
    if (CheckProbability(0.5) && x + 2 < CHUNK_SIZE && z - 2 > 0) chunk.SetBlockAt(BlockType::Leaves, x + 2, y, z - 2);
    if (CheckProbability(0.5) && x - 2 > 0 && z + 2 < CHUNK_SIZE) chunk.SetBlockAt(BlockType::Leaves, x - 2, y, z + 2);
}

void loadAssets(GLFWwindow* window)
{
    // Camera
    cam = new gfx::camera(60);

    // Textures
    atlas = new gfx::texture2D("res/atlas.png");
    atlas->load();

    // UI
    ImGui::CreateContext();
    ImGui_ImplOpenGL3_Init("#version 430 core");
    ImGui_ImplGlfw_InitForOpenGL(window, false);

    // Fonts
    fontHandler = new fonts::font_handler({
        {"Roboto", "res/Roboto-Regular.ttf", {14.f}}
    });

    // Chunks/World
    genChunks();
    // genFakeOctreeChunks();
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

lighting::LightingSettings* lightSettings;
void InitLightingSettings()
{
    lightSettings = new lighting::LightingSettings;
    lightSettings->LightColor = gfx::color("#E8FFFE");
    lightSettings->LightDir = glm::vec3(64.0f, -110.0f, 64.0f);
    lightSettings->FogColor = gfx::color("#7DADC4");
    lightSettings->FogDensity = 0.007;
}

gfx::color skyColor("#96e6ff");

lighting::DeferredRenderer* renderer;
void InitRenderer()
{
    renderer = new lighting::DeferredRenderer(mainWindow->width, mainWindow->height);
}

const char* renderModes = "Lit\0Position (G-Buffer)\0Normal (G-Buffer)\0Albedo (G-Buffer)\0AO (SSAO Pass)\0";
int chosenRenderMode = 0;

int ssaoBlurRadius = 2;
float ssaoRadius = 2.254f;
float ssaoBias = 0.384f;
float ssaoPower = 0.254f;
int ssaoSamples = 64;
bool lightingEnabled = false;
bool worldGenEnabled = false;
bool octreeDebugView = false;

float fps = 0;
int frameCount = 0;

int main(int argc, const char** argv)
{
    if (argc > 1) SeedProbability(atoi(argv[1]));
    else SeedProbability();
    mainWindow = new window::Window(800, 600, "Minecraft C++");
    
    loadAssets(mainWindow->GetHandle());
    InitLightingSettings();
    InitRenderer();

    glfwSetKeyCallback(mainWindow->GetHandle(), KeyCallback);
	glfwSetInputMode(mainWindow->GetHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    cam->camPos = glm::vec3(5.0f, 60.0f, 5.0f);

    mainWindow->AddResizeAction([&](int width, int height) {
        renderer->Resize(width, height);
        renderer->ssao->Resize(width, height);
    });

    double previousFrameTime = glfwGetTime();
    double deltaTime = 0.0f;
    mainWindow->AddRenderAction([&](GLFWwindow* window) {
        if (hide) CalculateLookAxis();
        CalculateMoveAxis(50.0f * deltaTime);
    });

    mainWindow->AddRenderAction([&](GLFWwindow* window) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);
        // Clear G-Buffer
        renderer->ClearGBuffer();
        // Render all chunks
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        renderer->gBufferShader->setInt("OctreeDebug", octreeDebugView ? 1 : 0);
        renderer->ssao->ssaoShader->setFloat("radius", ssaoRadius);
        renderer->ssao->ssaoShader->setFloat("bias", ssaoBias);
        renderer->ssao->ssaoShader->setInt("kernelSize", ssaoSamples);
        renderer->ssao->ssaoBlurShader->setInt("blurAmount", ssaoBlurRadius);
        // Draw chunks
        renderer->RenderWorld([&]() {
            auto meshRenderIt = meshRenderer.begin();
            while (meshRenderIt != meshRenderer.end())
            {
                (*meshRenderIt)->Draw();
                meshRenderIt++;
            }
        }, *cam);
        // Render lighting to screen
        mainWindow->SetClearColor(skyColor);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        renderer->lightingShader->setFloat("SSAOPower", ssaoPower);
        renderer->lightingShader->setInt("Debug", chosenRenderMode);
        renderer->RenderToScreen(*cam, *lightSettings);
    });

    // GUI
    mainWindow->AddRenderAction([&](GLFWwindow* window) {
        UI_BeginFrame();

        fontHandler->use("Roboto", 14.f);

        int indices = 0;
        auto meshRenderIt = meshRenderer.begin();
        while (meshRenderIt != meshRenderer.end())
        {
            (*meshRenderIt)->chunkData->indexCount;
            meshRenderIt++;
        }

        if (std::fmod(glfwGetTime(), 1.0) < deltaTime)
        {
            fps = 1.0 / deltaTime;
            frameCount = 0;
        }
        frameCount++;

        ImGui::Begin("Render Debug");
        ImGui::TextColored(ImVec4(UNPACK_COLOR3(gfx::color("#42aaf4")), 1.0), ("Chunks: " + std::to_string(meshRenderer.size())).c_str());
        ImGui::TextColored(ImVec4(UNPACK_COLOR3(gfx::color("#0ef174")), 1.0), ("FPS: " + std::to_string(static_cast<int>(fps))).c_str());
        ImGui::Separator();
        ImGui::Combo("Render Mode", &chosenRenderMode, renderModes);
        ImGui::Checkbox("Wireframe", &wireframe);
        ImGui::Checkbox("Octree Debug View", &octreeDebugView);
        ImGui::Separator();
        ImGui::Text("Game Seed: %d", GAME_SEED);
        ImGui::Text("Camera Position:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(UNPACK_COLOR4(gfx::color("#f44f41"))), "X: %0.f", cam->camPos.x);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(UNPACK_COLOR4(gfx::color("#56f441"))), "Y: %0.f", cam->camPos.y);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(UNPACK_COLOR4(gfx::color("#42cef4"))), "Z: %0.f", cam->camPos.z);
        ImGui::Separator();
        ImGui::Checkbox("Lighting", &lightingEnabled);
        ImGui::Checkbox("World Gen", &worldGenEnabled);
        ImGui::End();

        if (lightingEnabled)
        {
            ImGui::Begin("Lighting");
            ImGui::ColorEdit3("Light Color", (float*)&(lightSettings->LightColor));
            ImGui::SliderFloat3("Light Direction", glm::value_ptr(lightSettings->LightDir), -128.0, 128);
            ImGui::Separator();
            ImGui::ColorEdit3("Fog Color", (float*)&(lightSettings->FogColor));
            ImGui::SliderFloat("Fog Density", &(lightSettings->FogDensity), 0.0, 0.125);
            ImGui::Separator();
            ImGui::ColorEdit3("Sky Color", (float*)(&skyColor));
            ImGui::Separator();
            ImGui::SliderInt("SSAO Blur Radius", &ssaoBlurRadius, 2, 8);
            ImGui::SliderFloat("SSAO Radius", &ssaoRadius, 0.0, 10.0);
            ImGui::SliderFloat("SSAO Bias", &ssaoBias, 0.0, 1.0);
            ImGui::SliderFloat("SSAO Power", &ssaoPower, 0.1, 5.0);
            ImGui::SliderInt("SSAO Samples", &ssaoSamples, 1, 64);
            ImGui::End();
        }

        if (worldGenEnabled)
        {
            ImGui::Begin("World Gen");
            ImGui::Text("Noise Properties");
            ImGui::SliderInt("Octaves", &octaves, 1, 20);
            ImGui::SliderFloat("Frequency", &frequency, 0.0, 0.005, "%.6f");
            ImGui::SliderFloat("Lacunarity", &lacunarity, 0.0, 0.05);
            ImGui::SliderFloat("Bias", &bias, -1.0, 0.0);
            ImGui::SliderFloat("Upper Scale", &upperScale, 0.0, 2.0);
            ImGui::SliderFloat("Lower Scale", &lowerScale, 0.0, 2.0);
            ImGui::Separator();
            ImGui::Text("World Properties");
            ImGui::SliderInt("Base Height", &noiseBase, 15, 100);
            ImGui::SliderInt2("Chunk Amount", &ChunkDim[0], 1, 64);
            ImGui::SliderFloat("Tree Probability", &treeProb, 0.0, 0.1);
            ImGui::Separator();
            if (ImGui::Button("Regenerate World")) genChunks();
            ImGui::End();
        }

        fontHandler->pop_all();

        UI_EndFrame();
        deltaTime = glfwGetTime() - previousFrameTime;
        previousFrameTime = glfwGetTime();
    });

    mainWindow->Run();

    CleanupPointers();
    delete renderer, lightSettings, meshRenderer;

    return 0;
}