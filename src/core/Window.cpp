//
// Created by raph on 15/11/24.
//

#include "Window.h"
#include <stdexcept>

Window::Window(const WindowProperties& properties)
    : props(properties), window(nullptr) {
    init();
}

Window::~Window() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

void Window::init() {
    logger.info("Initializing GLFW");

    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW!");
    }

    glfwSetErrorCallback(errorCallback);

    // Tell GLFW not to create an OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, props.isResizable ? GLFW_TRUE : GLFW_FALSE);

    GLFWmonitor* monitor = props.isFullscreen ? glfwGetPrimaryMonitor() : nullptr;

    window = glfwCreateWindow(
        static_cast<int>(props.width),
        static_cast<int>(props.height),
        props.title.c_str(),
        monitor,
        nullptr
    );

    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window!");
    }

    // Store pointer to Window instance for callbacks
    glfwSetWindowUserPointer(window, this);

    logger.info("Window created successfully: " + std::to_string(props.width) + "x" +
                std::to_string(props.height));
}

void Window::update() {
    glfwPollEvents();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Window::close() {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

VkResult Window::createSurface(VkInstance instance, VkSurfaceKHR* surface) {
    return glfwCreateWindowSurface(instance, window, nullptr, surface);
}

std::pair<int, int> Window::getFramebufferSize() const {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return {width, height};
}

bool Window::isMinimized() const {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return width == 0 || height == 0;
}

void Window::setResizeCallback(GLFWframebuffersizefun callback) {
    glfwSetFramebufferSizeCallback(window, callback);
}

void Window::setKeyCallback(GLFWkeyfun callback) {
    glfwSetKeyCallback(window, callback);
}

void Window::setMouseButtonCallback(GLFWmousebuttonfun callback) {
    glfwSetMouseButtonCallback(window, callback);
}

void Window::setCursorPosCallback(GLFWcursorposfun callback) {
    glfwSetCursorPosCallback(window, callback);
}

void Window::setScrollCallback(GLFWscrollfun callback) {
    glfwSetScrollCallback(window, callback);
}

void Window::errorCallback(int error, const char* description) {
    Logger logger("GLFW");
    logger.error("GLFW Error (" + std::to_string(error) + "): " + description);
}

std::vector<const char *> Window::getRequiredExtensions() const {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}
