//
// Created by raph on 14/11/24.
//

#ifndef PIPELINE_H
#define PIPELINE_H

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <memory>
#include "../core/Logger.h"

class VulkanContext;
class Shader;

struct PipelineConfigInfo {
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
    VkPipelineMultisampleStateCreateInfo multisampleInfo{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
    VkPipelineViewportStateCreateInfo viewportInfo{};

    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
    std::vector<VkDynamicState> dynamicStates{};

    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;

    std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

    void enableDynamicStates(std::initializer_list<VkDynamicState> states) {
        dynamicStates = states;
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicStateInfo.pDynamicStates = dynamicStates.data();
    }
};

class Pipeline {
public:
    Pipeline(
        VulkanContext& context,
        const VkPipelineShaderStageCreateInfo shaderStages[2],
        const PipelineConfigInfo& configInfo);
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    static PipelineConfigInfo defaultPipelineConfigInfo();

    VkPipelineLayout getLayout() const { return pipelineLayout; }

private:
    void createGraphicsPipeline(
        const VkPipelineShaderStageCreateInfo shaderStages[2], const PipelineConfigInfo &configInfo);

    void createPipelineLayout(const PipelineConfigInfo& configInfo);

    VulkanContext& context;
    VkPipeline graphicsPipeline{};
    VkPipelineLayout pipelineLayout{};
    Logger logger;
};



#endif //PIPELINE_H
