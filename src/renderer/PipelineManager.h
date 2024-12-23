//
// Created by raph on 16/11/24.
//

#ifndef PIPELINEMANAGER_H
#define PIPELINEMANAGER_H

#include <unordered_map>
#include <memory>
#include <string>
#include <utility>
#include "Pipeline.h"
#include "Shader.h"

class VulkanContext;

class PipelineManager {
public:
    struct ShaderStages {
        std::unique_ptr<Shader> vertex;
        std::unique_ptr<Shader> fragment;
    };

    explicit PipelineManager(VulkanContext& context);
    ~PipelineManager();

    // Create a pipeline with a unique name
    void createPipeline(
        const std::string& name,
        const std::string& vertShaderPath,
        const std::string& fragShaderPath,
        const PipelineConfigInfo& configInfo
    );

    // Get a pipeline by name
    Pipeline* getPipeline(const std::string& name);

    // Get shader stages for a pipeline
    const ShaderStages* getShaderStages(const std::string& name) const;

    bool hasPipeline(const std::string& name) const;
    void removePipeline(const std::string& name);
    void clearPipelines();

    // Predefined configs
    static PipelineConfigInfo getDefaultConfig();
    static PipelineConfigInfo getUIConfig();
    static PipelineConfigInfo getTransparentConfig();
    static PipelineConfigInfo getParticleConfig();

    void recreatePipelines();

private:
    VulkanContext& context;
    std::unordered_map<std::string, std::unique_ptr<Pipeline>> pipelines;
    std::unordered_map<std::string, ShaderStages> shaderStages;
    std::unordered_map<std::string, PipelineConfigInfo> pipelineConfigs;
    std::unordered_map<std::string, std::pair<std::string, std::string>> shaderPaths;
    std::unordered_map<std::string, VkPipelineLayout> pipelineLayouts;

    void createPipelineLayout(const std::string& name, const PipelineConfigInfo& configInfo);
    void destroyPipelineLayout(const std::string& name);
    void createShaderStages(const std::string& name,
                           const std::string& vertPath,
                           const std::string& fragPath);
};


#endif //PIPELINEMANAGER_H
