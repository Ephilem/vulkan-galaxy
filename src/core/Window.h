//
// Created by raph on 15/11/24.
//

#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

#include "Logger.h"

struct WindowProperties {
    uint32_t width;
    uint32_t height;
    std::string title;
    bool isFullscreen;
    bool isResizable;

    WindowProperties(
        uint32_t w = 1280,
        uint32_t h = 720,
        const std::string& t = "Vulkan Application",
        bool fullscreen = false,
        bool resizable = false
    ) : width(w), height(h), title(t), isFullscreen(fullscreen), isResizable(resizable) {}
};

class Window {
public:
    explicit Window(const WindowProperties& props = WindowProperties());
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void update();
    bool shouldClose() const;
    void close();

    VkResult createSurface(VkInstance instance, VkSurfaceKHR* surface);
    std::vector<const char*> getRequiredExtensions() const;

    GLFWwindow* getHandle() const { return window; }
    uint32_t getWidth() const { return props.width; }
    uint32_t getHeight() const { return props.height; }
    std::pair<int, int> getFramebufferSize() const;
    bool isMinimized() const;

    void setResizeCallback(GLFWframebuffersizefun callback);
    void setKeyCallback(GLFWkeyfun callback);
    void setMouseButtonCallback(GLFWmousebuttonfun callback);
    void setCursorPosCallback(GLFWcursorposfun callback);
    void setScrollCallback(GLFWscrollfun callback);

private:
    void init();
    static void errorCallback(int error, const char* description);

    GLFWwindow* window;
    WindowProperties props;
    Logger logger{"Window"};
};
#endif //WINDOW_H
