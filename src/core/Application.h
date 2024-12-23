//
// Created by raph on 15/11/24.
//

#ifndef APPLICATION_H
#define APPLICATION_H


#include <array>
#include <memory>
#include "Window.h"
#include <glm/glm.hpp>
#include "Logger.h"
#include "../renderer/Buffer.h"

class Synchronization;
class PipelineManager;
class VulkanContext;
class SwapChain;
class CommandManager;
class Pipeline;

struct ApplicationConfig {
    WindowProperties windowProps;
    bool enableValidationLayers = true;
    uint32_t maxFramesInFlight = 2;
};

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

class Application {
public:
    explicit Application(const ApplicationConfig& config = ApplicationConfig());
    ~Application();

    // Delete copy constructor and operator
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // Core functions
    void run();
    void stop();

    // Getters
    Window& getWindow() { return *window; }
    VulkanContext& getVulkanContext() { return *vulkanContext; }

private:
    // Initialization
    void init();
    void initWindow();
    void initVulkan();
    void initCamera();

    // Frame handling
    void mainLoop();
    void update(float deltaTime);
    void render();
    void cleanup();

    // Event callbacks
    void onWindowResize(int width, int height);
    void onKeyEvent(int key, int scancode, int action, int mods);
    void onMouseMove(double xpos, double ypos);
    void onMouseButton(int button, int action, int mods);
    void onMouseScroll(double xoffset, double yoffset);

    // Member variables
    ApplicationConfig config;
    Logger logger{"Application"};

    bool isRunning;
    float lastFrameTime;

    // Core systems
    std::unique_ptr<Window> window;
    std::unique_ptr<VulkanContext> vulkanContext;
    std::unique_ptr<PipelineManager> pipelineManager;
    std::unique_ptr<Synchronization> synchronization;

    std::unique_ptr<Buffer> vertexBuffer;
    uint32_t vertexCount = 0;
    std::unique_ptr<Buffer> indexBuffer;
    uint32_t indexCount = 0;

    // Frame synchronization
    uint32_t currentFrame = 0;
};
#endif //APPLICATION_H
