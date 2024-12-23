//
// Created by raph on 14/11/24.
//

#ifndef SHADER_H
#define SHADER_H

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include "../core/Logger.h"

class VulkanContext;

class Shader {
public:
    enum class Type {
        Vertex,
        Fragment,
        Compute
    };

    Shader(VulkanContext& context, const std::string& filepath, Type type);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    VkShaderModule getShaderModule() const { return shaderModule; }
    Type getType() const { return type; }

private:
    static std::vector<char> readFile(const std::string& filepath);
    void createShaderModule(const std::vector<char>& code);

    VulkanContext& context;
    VkShaderModule shaderModule;
    Type type;
    Logger logger;
};


#endif //SHADER_H
