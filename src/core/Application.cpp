//
// Created by raph on 15/11/24.
//

#include "Application.h"
#include "../renderer/VulkanContext.h"
#include <chrono>

#include "../renderer/PipelineManager.h"
#include "../renderer/Synchronization.h"

Application::Application(const ApplicationConfig& config)
    : config(config)
    , logger("Application")
    , isRunning(false)
    , lastFrameTime(0.0f) {
    init();
}

Application::~Application() {
    cleanup();
}

void Application::init() {
    logger.info("Initializing application");

    initWindow();
    initVulkan();
    initCamera();

    synchronization = std::make_unique<Synchronization>(*vulkanContext, config.maxFramesInFlight);
    currentFrame = 0;

    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    // init vertex buffer
    vertexBuffer = std::make_unique<Buffer>(
        *vulkanContext,
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    vertexBuffer->copyFrom(vertices.data(), bufferSize);

    VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();
    indexBuffer = std::make_unique<Buffer>(
        *vulkanContext,
        indexBufferSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    indexBuffer->copyFrom(indices.data(), indexBufferSize);

    // Set up window callbacks
    window->setResizeCallback([](GLFWwindow* window, int width, int height) {
        auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->onWindowResize(width, height);
    });

    window->setKeyCallback([](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->onKeyEvent(key, scancode, action, mods);
    });

    window->setMouseButtonCallback([](GLFWwindow* window, int button, int action, int mods) {
        auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->onMouseButton(button, action, mods);
    });

    window->setCursorPosCallback([](GLFWwindow* window, double xpos, double ypos) {
        auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->onMouseMove(xpos, ypos);
    });

    window->setScrollCallback([](GLFWwindow* window, double xoffset, double yoffset) {
        auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->onMouseScroll(xoffset, yoffset);
    });
}

void Application::initWindow() {
    logger.info("Creating window");
    window = std::make_unique<Window>(config.windowProps);
}

void Application::initVulkan() {
    logger.info("Initializing Vulkan");
    vulkanContext = std::make_unique<VulkanContext>(*window);
    vulkanContext->initialize();

    vulkanContext->getCommandManager().createCommandBuffers(config.maxFramesInFlight);

    pipelineManager = std::make_unique<PipelineManager>(*vulkanContext);

    auto basicConfig = PipelineManager::getDefaultConfig();
    basicConfig.bindingDescriptions = {Vertex::getBindingDescription()};
    auto attributes = Vertex::getAttributeDescriptions();
    basicConfig.attributeDescriptions = std::vector<VkVertexInputAttributeDescription>(attributes.begin(), attributes.end());

    pipelineManager->createPipeline(
        "basic",
        "shaders/shader.vert.spv",
        "shaders/shader.frag.spv",
        basicConfig
    );
}

void Application::initCamera() {
    // camera = std::make_unique<Camera>(
    //     glm::vec3(0.0f, 0.0f, -10.0f),  // position
    //     glm::vec3(0.0f, 0.0f, 0.0f),    // target
    //     glm::vec3(0.0f, 1.0f, 0.0f)     // up
    // );
}

void Application::run() {
    logger.info("Starting application main loop");
    isRunning = true;
    lastFrameTime = static_cast<float>(glfwGetTime());

    while (isRunning && !window->shouldClose()) {
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        update(deltaTime);
        render();
    }

    // Wait for the GPU to finish all operations
    vulkanContext->waitIdle();
}

void Application::update(float deltaTime) {
    window->update();
    // camera->update(deltaTime);
}

void Application::render() {
    if (window->isMinimized()) {
        return;
    }

    // Wait for the previous frame to complete
    synchronization->waitForFence(currentFrame);

    // Get command buffer for current frame
    auto& commandManager = vulkanContext->getCommandManager();
    commandManager.setCurrentFrame(currentFrame);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        vulkanContext->getDevice(),
        vulkanContext->getSwapChain().swapChain,
        UINT64_MAX,
        synchronization->getImageAvailableSemaphore(currentFrame),
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        vulkanContext->getSwapChain().recreate();
        return;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    // Reset the command buffer only after we're sure the previous frame is done
    synchronization->resetFence(currentFrame);

    VkCommandBuffer commandBuffer = commandManager.getCurrentBuffer();
    vkResetCommandBuffer(commandBuffer, 0);

    // Begin command buffer recording
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    // Begin render pass
    auto& swapChain = vulkanContext->getSwapChain();
    swapChain.beginRenderPass(commandBuffer, swapChain.getFramebuffers()[imageIndex]);

    if (pipelineManager && pipelineManager->hasPipeline("basic")) {
        auto* pipeline = pipelineManager->getPipeline("basic");
        pipeline->bind(commandBuffer);
        vertexBuffer->bindAsVertex(commandBuffer);
        indexBuffer->bindAsIndex(commandBuffer, 1);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain.getExtent().width);
        viewport.height = static_cast<float>(swapChain.getExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChain.getExtent();
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        // Draw call would go here
        vkCmdDraw(commandBuffer, 3, 1, 0, 0); // Draw a triangle for testing
    }

    // End render pass
    swapChain.endRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }

    // Submit command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {synchronization->getImageAvailableSemaphore(currentFrame)};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = {synchronization->getRenderFinishedSemaphore(currentFrame)};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vulkanContext->getGraphicsQueue(), 1, &submitInfo, synchronization->getFence(currentFrame)) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }

    // Present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain.swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(vulkanContext->getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        swapChain.recreate();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    // Update current frame
    currentFrame = (currentFrame + 1) % config.maxFramesInFlight;
}

void Application::stop() {
    isRunning = false;
}

void Application::cleanup() {
    logger.info("Cleaning up application");

    synchronization.reset();
    pipelineManager.reset();
    vertexBuffer.reset();
    indexBuffer.reset();
    vulkanContext.reset();
    window.reset();
}

// Event handling implementations
void Application::onWindowResize(int width, int height) {
    if (width == 0 || height == 0) return;

    // Handle resize
    // This might involve recreating the swap chain
}

void Application::onKeyEvent(int key, int scancode, int action, int mods) {
    // Handle keyboard input
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        stop();
    }

    // Pass input to camera
    // camera->handleKeyInput(key, action);
}

void Application::onMouseMove(double xpos, double ypos) {
    // camera->handleMouseMove(static_cast<float>(xpos), static_cast<float>(ypos));
}

void Application::onMouseButton(int button, int action, int mods) {
    // camera->handleMouseButton(button, action);
}

void Application::onMouseScroll(double xoffset, double yoffset) {
    // camera->handleMouseScroll(static_cast<float>(yoffset));
}