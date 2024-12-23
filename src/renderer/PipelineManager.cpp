//
// Created by raph on 16/11/24.
//

#include "PipelineManager.h"
#include "VulkanContext.h"

PipelineManager::PipelineManager(VulkanContext& context)
    : context(context) {}

PipelineManager::~PipelineManager() {
    clearPipelines();
    shaderStages.clear();
}

void PipelineManager::createShaderStages(
    const std::string& name,
    const std::string& vertPath,
    const std::string& fragPath) {

    ShaderStages stages;
    stages.vertex = std::make_unique<Shader>(context, vertPath, Shader::Type::Vertex);
    stages.fragment = std::make_unique<Shader>(context, fragPath, Shader::Type::Fragment);

    shaderStages[name] = std::move(stages);
}

void PipelineManager::createPipeline(
    const std::string& name,
    const std::string& vertShaderPath,
    const std::string& fragShaderPath,
    const PipelineConfigInfo& configInfo) {

    if (hasPipeline(name)) {
        throw std::runtime_error("Pipeline with name '" + name + "' already exists");
    }

    // Store configurations for recreation
    shaderPaths[name] = {vertShaderPath, fragShaderPath};
    pipelineConfigs[name] = configInfo;

    // Create shader stages
    createShaderStages(name, vertShaderPath, fragShaderPath);

    // Create pipeline layout
    createPipelineLayout(name, configInfo);

    // Update config with the created layout
    auto finalConfig = configInfo;
    finalConfig.pipelineLayout = pipelineLayouts[name];
    finalConfig.renderPass = context.getSwapChain().getRenderPass();

    // Create shader stage create infos
    VkPipelineShaderStageCreateInfo shaderStages[2]{};
    
    // Vertex shader stage
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = this->shaderStages[name].vertex->getShaderModule();
    shaderStages[0].pName = "main";

    // Fragment shader stage
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = this->shaderStages[name].fragment->getShaderModule();
    shaderStages[1].pName = "main";

    // Create the pipeline
    // Note: You'll need to modify your Pipeline constructor to accept shader stages
    pipelines[name] = std::make_unique<Pipeline>(
        context,
        shaderStages,
        finalConfig
    );
}

Pipeline* PipelineManager::getPipeline(const std::string& name) {
    if (!hasPipeline(name)) {
        throw std::runtime_error("Pipeline '" + name + "' not found");
    }
    return pipelines[name].get();
}

const PipelineManager::ShaderStages* PipelineManager::getShaderStages(const std::string& name) const {
    const auto it = shaderStages.find(name);
    if (it != shaderStages.end()) {
        return &it->second;
    }
    return nullptr;
}

bool PipelineManager::hasPipeline(const std::string& name) const {
    return pipelines.contains(name);
}

void PipelineManager::removePipeline(const std::string& name) {
    if (!hasPipeline(name)) {
        return;
    }

    pipelines.erase(name);
    destroyPipelineLayout(name);
    shaderPaths.erase(name);
    pipelineConfigs.erase(name);
}

void PipelineManager::clearPipelines() {
    pipelines.clear();

    // Destroy all pipeline layouts
    for (const auto& [name, layout] : pipelineLayouts) {
        vkDestroyPipelineLayout(context.getDevice(), layout, nullptr);
    }
    pipelineLayouts.clear();

    shaderPaths.clear();
    pipelineConfigs.clear();
}

void PipelineManager::recreatePipelines() {
    // Store current pipelines and configs
    auto currentShaderPaths = shaderPaths;
    auto currentConfigs = pipelineConfigs;

    // Clear existing pipelines
    clearPipelines();

    // Recreate all pipelines
    for (const auto& [name, paths] : currentShaderPaths) {
        createPipeline(name, paths.first, paths.second, currentConfigs[name]);
    }
}

void PipelineManager::createPipelineLayout(const std::string& name, const PipelineConfigInfo& configInfo) {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    // Add descriptor set layouts if they exist
    // pipelineLayoutInfo.setLayoutCount = configInfo.descriptorSetLayouts.size();
    // pipelineLayoutInfo.pSetLayouts = configInfo.descriptorSetLayouts.data();

    // Add push constant ranges if they exist
    // pipelineLayoutInfo.pushConstantRangeCount = configInfo.pushConstantRanges.size();
    // pipelineLayoutInfo.pPushConstantRanges = configInfo.pushConstantRanges.data();

    if (vkCreatePipelineLayout(context.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayouts[name]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout for '" + name + "'");
    }
}

void PipelineManager::destroyPipelineLayout(const std::string& name) {
    if (pipelineLayouts.find(name) != pipelineLayouts.end()) {
        vkDestroyPipelineLayout(context.getDevice(), pipelineLayouts[name], nullptr);
        pipelineLayouts.erase(name);
    }
}

PipelineConfigInfo PipelineManager::getDefaultConfig() {
    return Pipeline::defaultPipelineConfigInfo();
}

PipelineConfigInfo PipelineManager::getUIConfig() {
    auto config = getDefaultConfig();

    // desactivate culling for UI elements
    config.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;

    // alpha blending
    config.colorBlendAttachment.blendEnable = VK_TRUE;
    config.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    config.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    config.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    config.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    config.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    config.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    return config;
}

PipelineConfigInfo PipelineManager::getTransparentConfig() {
    auto config = getDefaultConfig();

    // enable alpha blending
    config.colorBlendAttachment.blendEnable = VK_TRUE;
    config.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    config.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    config.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;

    // Enable depth test but disable depth write
    config.depthStencilInfo.depthTestEnable = VK_TRUE;
    config.depthStencilInfo.depthWriteEnable = VK_FALSE;

    return config;
}

PipelineConfigInfo PipelineManager::getParticleConfig() {
    auto config = getTransparentConfig();

    return config;
}

