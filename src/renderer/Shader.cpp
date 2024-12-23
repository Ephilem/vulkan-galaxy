//
// Created by raph on 14/11/24.
//

#include "Shader.h"
#include "VulkanContext.h"
#include <fstream>

Shader::Shader(VulkanContext& context, const std::string& filepath, Type type)
    : context(context), type(type), logger("Shader") {
    auto code = readFile(filepath);
    createShaderModule(code);
}

Shader::~Shader() {
    vkDestroyShaderModule(context.getDevice(), shaderModule, nullptr);
}

std::vector<char> Shader::readFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file: " + filepath);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

void Shader::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(context.getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
}

